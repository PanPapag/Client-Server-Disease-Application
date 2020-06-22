#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wordexp.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../../common/includes/constants.h"
#include "../../common/includes/file_utils.h"
#include "../../common/includes/hash_table.h"
#include "../../common/includes/io_utils.h"
#include "../../common/includes/ipv4_socket.h"
#include "../../common/includes/list.h"
#include "../../common/includes/macros.h"
#include "../../common/includes/message.h"
#include "../../common/includes/report_utils.h"
#include "../../common/includes/statistics.h"
#include "../../common/includes/string_utils.h"
#include "../../common/includes/types.h"
#include "../../common/includes/utils.h"

#include "../includes/avl.h"
#include "../includes/commands.h"
#include "../includes/io_utils.h"
#include "../includes/patient_record.h"
#include "../includes/utils.h"

worker_structures structures;

worker_options options;

ipv4_socket server_socket;
ipv4_socket worker_socket;

void create_global_data_structures(void) {
  structures.patient_record_ht = hash_table_create(NO_BUCKETS, BUCKET_SIZE,
                                                   string_hash, string_compare,
                                                   string_print, patient_record_print,
                                                   NULL, patient_record_destroy);

  structures.disease_ht = hash_table_create(NO_BUCKETS, BUCKET_SIZE,
                                            string_hash, string_compare,
                                            string_print, avl_print_inorder,
                                            NULL, avl_clear);

  structures.country_ht = hash_table_create(NO_BUCKETS, BUCKET_SIZE,
                                            string_hash, string_compare,
                                            string_print, avl_print_inorder,
                                            NULL, avl_clear);

  structures.file_paths_ht = hash_table_create(NO_BUCKETS, BUCKET_SIZE,
                                               string_hash, string_compare,
                                               string_print, string_print,
                                               string_destroy, NULL);

  structures.countries_names = list_create(string*, ptr_to_string_compare,
                                           ptr_to_string_print, NULL);

	structures.diseases_names = list_create(string*, ptr_to_string_compare,
                                          ptr_to_string_print, NULL);

  structures.files_statistics = list_create(statistics_entry_ptr*, NULL,
                                            ptr_to_statistics_entry_print,
                                            ptr_to_statistics_entry_destroy);
}

static
hash_table_ptr __parse_file_and_update_global_structures(const char *dir_name,
                                                         const char *file_path,
                                                         const char *file_name) {
  char buffer[MAX_BUFFER_SIZE];
  char *patient_record_tokens[NO_PATIENT_RECORD_TOKENS];
  char **file_entry_tokens;
  int file_entry_no_tokens;
  wordexp_t p;
  patient_record_ptr patient_record;
  // Open file for read only - handles binary fille too
  FILE *fp = fopen(file_path, "rb+");
  // age_groups_ht: disease_id --> array of int to store total cases per age group
  hash_table_ptr age_groups_ht = hash_table_create(NO_BUCKETS, BUCKET_SIZE,
                                                   string_hash, string_compare,
                                                   string_print, age_groups_print,
                                                   NULL, age_groups_destroy);
  // Read file line by line
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    // Discard '\n' that fgets() stores
    buffer[strlen(buffer) - 1] = '\0';
    wordexp(buffer, &p, 0);
    file_entry_tokens = p.we_wordv;
    file_entry_no_tokens = p.we_wordc;
    char *status = file_entry_tokens[1];
    if (!strcmp(status,"ENTER")) {
      patient_record = patient_record_create(file_entry_tokens, file_name, dir_name);
      execute_insert_patient_record(patient_record, age_groups_ht);
    } else {
      execute_record_patient_exit(file_entry_tokens[0], file_name);
    }
    // Free wordexp object
    wordfree(&p);
  }
  // Close file pointer
  fclose(fp);
  // return age_groups_ht of the file that just being parsed
  return age_groups_ht;
}

static
void __parse_directory_and_update_global_structures(const char *dir_path) {
  hash_table_ptr age_groups_ht;
  statistics_entry_ptr statistics_entry;
  struct dirent *direntp;
  DIR *dir_ptr = opendir(dir_path);
  if (dir_ptr == NULL) {
    die("Worker with pid <%ld> could not open <%s>. Exiting ...", (long) getpid(), dir_path);
  }
  else {
    const char *dir_name = get_last_token(dir_path, '/');
    while ((direntp = readdir(dir_ptr)) != NULL) {
      // Skip previous and current folder
      if (strcmp(direntp->d_name, ".") && strcmp(direntp->d_name, "..")) {
        // Allocatte memory to save the constructed file_path
        size_t file_path_size = strlen(dir_path) + strlen(direntp->d_name) + 2;
        char *file_path = __MALLOC__(file_path_size, char);
        if (file_path == NULL) {
          die("Worker with pid <%ld> could not allocate memory for the file"
              " path construction. Exiting ...", (long) getpid());
        }
        // Construct full file path
        snprintf(file_path, file_path_size, "%s/%s", dir_path, direntp->d_name);
        // Insert file path to file_paths_ht
        hash_table_insert(&structures.file_paths_ht, file_path, file_path);
        // The parsing of the file and the update of the structures is executed
        // by the function below
        age_groups_ht = __parse_file_and_update_global_structures(dir_name,
                                                                  file_path,
                                                                  direntp->d_name);
        // Create a statistics entry for the current file
        // Store each statistics entry to a global list in order to send them via
        // fifo to the disease aggregator
        statistics_entry = statistics_entry_create(direntp->d_name, dir_name, age_groups_ht);
        list_push_back(&structures.files_statistics, &statistics_entry);
      }
    }
    closedir(dir_ptr);
  }
}

static inline
void __handle_command(char *command) {
  wordexp_t p;
  char **command_tokens;
  int command_no_tokens;
  char **command_argv;
  int command_argc;
  // Use API POSIX to extract arguments
  wordexp(command, &p, 0);
  command_tokens = p.we_wordv;
  command_no_tokens = p.we_wordc;
  // Call correspoding command function
  if (!strcmp(command_tokens[0], "/diseaseFrequency")) {
    command_argv = prune_command_name(command_tokens, command_no_tokens);
    command_argc = command_no_tokens - 1;
    execute_disease_frequency(command_argc, command_argv);
    __FREE__(command_argv);
  } else if (!strcmp(command_tokens[0], "/topk-AgeRanges")) {
    command_argv = prune_command_name(command_tokens, command_no_tokens);
    execute_topk_age_ranges(command_argv);
  } else if (!strcmp(command_tokens[0], "/searchPatientRecord")) {
    command_argv = prune_command_name(command_tokens, command_no_tokens);
    execute_search_patient_record(command_argv);
    __FREE__(command_argv);
  } else if (!strcmp(command_tokens[0], "/numPatientAdmissions")) {
    command_argv = prune_command_name(command_tokens, command_no_tokens);
    command_argc = command_no_tokens - 1;
    execute_num_patients_admissions(command_argc, command_argv);
    __FREE__(command_argv);
  } else if (!strcmp(command_tokens[0], "/numPatientDischarges")) {
    command_argv = prune_command_name(command_tokens, command_no_tokens);
    command_argc = command_no_tokens - 1;
    execute_num_patients_discharges(command_argc, command_argv);
    __FREE__(command_argv);
  }
  // Free wordexp object
  wordfree(&p);
}

void parse_dirs_and_update_global_data_structures(void) {
  // Open named pipe to read the directory paths
  int read_fd = open(options.fifo, O_RDONLY);
  if (read_fd < 0) {
    die("Worker with pid <%ld> could not open named pipe: <%s>. Exiting ../",
        (long) getpid(), options.fifo);
  }
  // Read from the pipe the director paths
  options.dir_paths = read_in_chunks(read_fd, options.buffer_size);
  // Tokenize options.dir_paths string to extract directories to parse
  char dir_paths[MAX_BUFFER_SIZE];
  strcpy(dir_paths, options.dir_paths);
  char *dir_path = strtok(dir_paths, SPACE);
	while (dir_path != NULL) {
    __parse_directory_and_update_global_structures(dir_path);
		dir_path = strtok(NULL, SPACE);
	}
}

void setup_worker_socket(void) {
  if (ipv4_socket_create(0, IPV4_ANY_ADDRESS, &worker_socket) < 0) {
    die("Could not create address for server to worker socket!");
  }
  if (ipv4_socket_bind(&worker_socket) < 0) {
    die("Could not bind server address to worker socket!");
  }
  if (ipv4_socket_listen(&worker_socket) < 0) {
    die("Could not register server address for listening to worker socket!");
  }
}

void send_hostname_and_port_to_server(void) {
  // Get hostname
  char hostname[MAX_BUFFER_SIZE];
  gethostname(hostname, sizeof(hostname));
  // Get port number automatically assigned
  uint16_t port;
  ipv4_socket_get_port(&worker_socket, &port);
  // Send hostname and port to server
  ipv4_socket hostname_port_socket;
  message message;
  if (ipv4_socket_create_and_connect(options.server_ip, options.server_port_number, &hostname_port_socket)) {
    char buffer[MAX_BUFFER_SIZE];
    sprintf(buffer, "%s %d", hostname, port);
    message = message_create(buffer, HOSTNAME_AND_PORT);
    if (!ipv4_socket_send_message(&hostname_port_socket, message)) {
      report_warning("Message <%s> could not be sent to server!", (char*) message.data);
    }
    message_destroy(&message);
  }
}

void send_statistics_to_server(void) {
  ipv4_socket statistics_socket;
  message message;
	if (ipv4_socket_create_and_connect(options.server_ip, options.server_port_number, &statistics_socket)) {
    // Send how many file statistics have to be sent
    char num_statistics[12];
    sprintf(num_statistics, "%ld", list_size(structures.files_statistics));
    message = message_create(num_statistics, NUM_STATISTICS);
    if (!ipv4_socket_send_message(&statistics_socket, message)) {
      report_warning("Message <%s> could not be sent to server!", (char*) message.data);
    }
    message_destroy(&message);
    // Send actual statistics
    for (size_t i = 1U; i <= list_size(structures.files_statistics); ++i) {
      list_node_ptr list_node = list_get(structures.files_statistics, i);
      char *serialized_statistics_entry = ptr_to_statistics_entry_serialize(
                                            list_node->data_,
                                            structures.diseases_names);
      message = message_create(serialized_statistics_entry, STATISTICS);
  		if (!ipv4_socket_send_message(&statistics_socket, message)) {
  			report_warning("Message <%s> could not be sent to server!", (char*) message.data);
  		}
      message_destroy(&message);
      __FREE__(serialized_statistics_entry);
    }
	}
}

void start_worker_as_server(void) {
  message message;
  while (1) {
    ipv4_socket_accept(&worker_socket, &server_socket);
    message = ipv4_socket_get_message(&server_socket);
    __handle_command((char*) message.data);
    close(server_socket.socket_fd);
  }
}

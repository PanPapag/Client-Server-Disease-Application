#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "../../common/includes/constants.h"
#include "../../common/includes/file_utils.h"
#include "../../common/includes/io_utils.h"
#include "../../common/includes/list.h"
#include "../../common/includes/macros.h"
#include "../../common/includes/report_utils.h"
#include "../../common/includes/string_utils.h"
#include "../../common/includes/types.h"

#include "../includes/io_utils.h"
#include "../includes/utils.h"

master_options options;

void spawn_workers(void) {
  // Convert command line argument buffer_size to string
  char buffer_size[12];
  sprintf(buffer_size, "%d", options.buffer_size);
  // Convert server port number to string
  char server_port_number[12];
  sprintf(server_port_number, "%d", options.server_port_number);
  // Fifo array of char holds the name of named pipe via master and worker communicates
  char fifo[FIFO_NAME_SIZE];
  // Store in a variable of type pid_t the pid of each worker proccess spawned
  pid_t pid;
  for (size_t i = 0U; i < options.num_workers; ++i) {
    pid = fork();
    // Handle fork failure
    if (pid == -1) {
      perror("Failed to fork!");
      exit(EXIT_FAILURE);
    }
    if (pid == 0) {
      // Create named pipe in which parent writes and child reads
      sprintf(fifo, "fifo_%d", getpid());
      int ret_val = mkfifo(fifo, 0666);
      if ((ret_val == -1) && (errno != EEXIST)) {
         perror("Creating Fifo Failed");
         exit(1);
      }
      // Start worker executable passing its arguments
      execl("./worker", "worker", "-f", fifo, "-b", buffer_size,
            "-s", options.server_ip, "-p", server_port_number, (char *) NULL);
      perror("execl() Execution Failed");
      exit(EXIT_FAILURE);
    } else {
      // Parent just stores in master_options workers_pid array the pids of workers
      options.workers_pid[i] = pid;
    }
  }
}

static void __update_worker(pid_t pid, int pos) {
  char workers_fifo[FIFO_NAME_SIZE];
  // Store worker pid to global options/
  options.workers_pid[pos] = pid;
  // Reconstruct named pipe given the children proccesses ids
  sprintf(workers_fifo, "fifo_%d", options.workers_pid[pos]);
  /*
    Open named pipe for writing the directories paths to be sent as well as
    distributing the application commands given from stdin
  */
  options.workers_fd[pos] = open(workers_fifo, O_WRONLY);
  if (options.workers_fd[pos] < 0) {
    die("<Master> could not open named pipe: %s", workers_fifo);
  }
  printf("MASTER: %s\n", options.workers_dir_paths[pos]);
  // Write to the pipe the directories paths
  write_in_chunks(options.workers_fd[pos], options.workers_dir_paths[pos], options.buffer_size);
}

void update_workers(void) {
  options.workers_fd = __MALLOC__(options.num_workers, int);
  for (size_t i = 0U; i < options.num_workers; ++i) {
    __update_worker(options.workers_pid[i], i);
  }
}

static
char** __distribute_subdirs(list_ptr directories) {
  size_t num_directories = list_size(directories);
  char **result = __MALLOC__(options.num_workers, char*);
  /*
    Construct an array with index the worker number and value the number of
    directories which he is going to handle
  */
  int no_dirs_per_worker[options.num_workers];
  memset(no_dirs_per_worker, 0, sizeof(no_dirs_per_worker));
  /* Case 1: num_workers > num_directories */
  if (options.num_workers > num_directories) {
    for (size_t i = 0U; i < options.num_workers; ++i) {
      if (i == num_directories)
        break;
      no_dirs_per_worker[i] = 1;
    }
  } else {
    /* Case 2: num_workers <= num_directories */
    for (size_t i = 0U; i < options.num_workers; ++i) {
      no_dirs_per_worker[i] = num_directories / options.num_workers;
    }
    for (size_t i = 0U; i < num_directories % options.num_workers; ++i) {
      no_dirs_per_worker[i]++;
    }
  }
  /* Copy directories names to result array */
  for (size_t i = 0U; i < options.num_workers; ++i) {
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    for (size_t j = 0U; j < no_dirs_per_worker[i]; ++j) {
      list_node_ptr list_node = list_pop_front(&directories);
      char *dir_name = (*(char **) list_node->data_);
      strcat(buffer, dir_name);
      if (j < no_dirs_per_worker[i] - 1) {
        strcat(buffer, SPACE);
      }
      /* Clear memory */
      __FREE__(list_node);
      __FREE__(dir_name);
    }
    result[i] = __MALLOC__(strlen(buffer)+1, char);
    strcpy(result[i], buffer);
  }
  return result;
}


void get_and_distribute_subdirs(void) {
  list_ptr subdirs = get_subdirs(options.input_dir);
  options.workers_dir_paths = __distribute_subdirs(subdirs);
  list_clear(subdirs);
}

int get_worker_fd_pos(pid_t pid) {
  for (int i = 0; i < options.num_workers; ++i) {
    if (options.workers_pid[i] == pid) return i;
  }
  return -1;
}

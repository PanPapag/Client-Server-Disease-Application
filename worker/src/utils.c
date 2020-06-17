#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../../common/includes/constants.h"
#include "../../common/includes/file_utils.h"
#include "../../common/includes/hash_table.h"
#include "../../common/includes/io_utils.h"
#include "../../common/includes/list.h"
#include "../../common/includes/macros.h"
#include "../../common/includes/report_utils.h"
#include "../../common/includes/statistics.h"
#include "../../common/includes/string_utils.h"
#include "../../common/includes/types.h"
#include "../../common/includes/utils.h"

#include "../includes/avl.h"
#include "../includes/io_utils.h"
#include "../includes/patient_record.h"
#include "../includes/utils.h"

worker_structures structures;

worker_options options;

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


void parse_dirs_and_update_global_data_structures(void) {
  // Open named pipe to read the directory paths
  int read_fd = open(options.fifo, O_RDONLY);
  if (read_fd < 0) {
    die("Worker with pid <%ld> could not open named pipe: %s", (long)getpid(),
                                                               options.fifo);
  }
  // Read from the pipe the director paths
  options.dir_paths = read_in_chunks(read_fd, options.buffer_size);
  // Tokenize options.dir_paths string to extract directories to parse
  char dir_paths[MAX_BUFFER_SIZE];
  strcpy(dir_paths, options.dir_paths);
  char *dir_path = strtok(dir_paths, SPACE);
	while (dir_path != NULL) {
    printf("%s\n", dir_path);
		dir_path = strtok(NULL, SPACE);
	}
}

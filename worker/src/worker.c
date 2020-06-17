#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../../common/includes/io_utils.h"

#include "../includes/io_utils.h"
#include "../includes/utils.h"

worker_options options;
worker_structures structures;

int main(int argc, char *argv[]) {
  parse_command_line_arguments(argc, argv);
  create_global_data_structures();
  /* Open named pipe to read the directories paths as well as the application commands */
  int read_fd = open(options.fifo, O_RDONLY);
  if (read_fd < 0) {
    printf("SKATA\n");
    exit(EXIT_FAILURE);
 }
  /* Read from the pipe the directories paths and parse them */
  char *dir_paths = read_in_chunks(read_fd, options.buffer_size);
  printf("WORKER: %s\n", dir_paths);
  return EXIT_SUCCESS;
}

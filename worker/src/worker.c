#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../includes/io_utils.h"

worker_options options;

int main(int argc, char* argv[]) {
  parse_command_line_arguments(argc, argv);
  printf("%s\n",options.fifo);
  return EXIT_SUCCESS;
}

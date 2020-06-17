#include <stdio.h>
#include <stdlib.h>

#include "../includes/io_utils.h"

server_options options;

int main(int argc, char *argv[]) {
  parse_command_line_arguments(argc, argv);
  return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/io_utils.h"
#include "../includes/utils.h"

worker_options options;

int main(int argc, char *argv[]) {
  parse_command_line_arguments(argc, argv);
  create_global_data_structures();
  parse_dirs_and_update_global_data_structures();
  return EXIT_SUCCESS;
}

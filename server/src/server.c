#include <stdio.h>
#include <stdlib.h>

#include "../includes/io_utils.h"
#include "../includes/pool.h"
#include "../includes/utils.h"

server_options options;

pool_t pool;

int main(int argc, char *argv[]) {
  parse_command_line_arguments(argc, argv);
  setup_server_connections();
  create_global_data_structures();
  create_threads_and_start_server();
  return EXIT_SUCCESS;
}

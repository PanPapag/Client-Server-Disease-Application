#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/io_utils.h"
#include "../includes/utils.h"

int main(int argc, char *argv[]) {
  parse_command_line_arguments(argc, argv);
  create_global_data_structures();
  parse_dirs_and_update_global_data_structures();
  setup_worker_socket();
  send_hostname_and_port_to_server();
  send_statistics_to_server();
  return EXIT_SUCCESS;
}

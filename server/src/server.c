#include <stdio.h>
#include <stdlib.h>

#include "../../common/includes/ipv4_socket.h"

#include "../includes/io_utils.h"
#include "../includes/pool.h"
#include "../includes/utils.h"

server_options options;

pool_t pool;

int main(int argc, char *argv[]) {
  parse_command_line_arguments(argc, argv);
  initialize_pool(&pool, options.buffer_size);
  create_threads_and_start_server();
  destroy_pool(&pool);
  return EXIT_SUCCESS;
}

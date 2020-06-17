#ifndef __WORKER_IO_UTILS__
  #define __WORKER_IO_UTILS__

  #include <stdint.h>
  #include <getopt.h>

  #include "../../common/includes/attributes.h"

  static struct option options_spec[] = {
    {"f",    required_argument, NULL, 'f'},
    {"b",    required_argument, NULL, 'b'},
    {"s",    required_argument, NULL, 's'},
    {"p",    required_argument, NULL, 'p'},
    {"help", no_argument,       NULL, 'h'},
    {0, 0, 0, 0}
  };

  typedef struct worker_options {
    uint16_t buffer_size;
    uint16_t server_port_number;
    char *fifo;
    char *server_ip;
    char *dir_paths;
  } worker_options;

  extern worker_options options;

  /**
    \brief Parses the command line arguments of worker executable and
      updates the worker_options struct
    @param argc: Number of the command line arguments
    @param argv: Array of the command lines arguments
  */
  __NON_NULL__(2)
  void parse_command_line_arguments(int argc, char *argv[]);

#endif

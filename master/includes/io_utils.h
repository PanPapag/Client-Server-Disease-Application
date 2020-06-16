#ifndef _MASTER_IO_UTILS__
	#define __MASTER_IO_UTILS__

	#include <stdint.h>
	#include <getopt.h>

	#include "../../common/includes/attributes.h"

  static struct option options_spec[] = {
    {"w",    required_argument, NULL, 'w'},
    {"b",    required_argument, NULL, 'b'},
    {"s",    required_argument, NULL, 's'},
    {"p",    required_argument, NULL, 'p'},
    {"i",    required_argument, NULL, 'i'},
    {"help", no_argument,       NULL, 'h'},
    {0, 0, 0, 0}
  };

  typedef struct master_options {
    uint16_t num_workers;
    uint16_t buffer_size;
    uint16_t server_port_number;
    char *server_ip;
    char *input_dir;
  } master_options;

	/**
		\brief Parses the command line arguments of master executable and
			updates the master_options struct
		@param argc: Number of the command line arguments
		@param argv: Array of the command lines arguments
	*/
	__NON_NULL__(2)
	master_options parse_command_line_arguments(int argc, char *argv[]);


#endif

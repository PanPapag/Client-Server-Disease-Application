#ifndef __SERVER_IO_UTILS__
	#define __SERVER_IO_UTILS__

	#include <stdint.h>
	#include <getopt.h>

	#include "../../common/includes/attributes.h"
	#include "../../common/includes/list.h"
	#include "../../common/includes/ipv4_socket.h"

  static struct option options_spec[] = {
    {"q",    required_argument, NULL, 'q'},
    {"s",    required_argument, NULL, 's'},
    {"w",    required_argument, NULL, 'w'},
    {"b",    required_argument, NULL, 'b'},
    {"help", no_argument,       NULL, 'h'},
    {0, 0, 0, 0}
  };

  typedef struct server_options {
    uint16_t num_threads;
    uint16_t buffer_size;
    uint16_t query_port_number;
    uint16_t statistics_port_number;
		char *workers_ip_address;
  } server_options;

	extern ipv4_socket server_statistics_socket;
	extern ipv4_socket server_query_socket;

	/**
		\brief Parses the command line arguments of server executable and
			updates the server_options struct
		@param argc: Number of the command line arguments
		@param argv: Array of the command lines arguments
	*/
	__NON_NULL__(2)
	void parse_command_line_arguments(int argc, char *argv[]);

#endif

#ifndef __CLIENT_IO_UTILS__
	#define __CLIENT_IO_UTILS__

	#include <stdint.h>
	#include <getopt.h>

	#include "../../common/includes/attributes.h"

	static struct option options_spec[] = {
		{"q", required_argument, NULL, 'q'},
		{"w", required_argument, NULL, 'w'},
		{"sp", required_argument, NULL, 's'},
		{"sip", required_argument, NULL, 'i'},
		{0, 0, 0, 0}
	};

	typedef struct client_options {
		char *query_file;
    char *server_ip;
    uint16_t server_port_number;
    uint16_t num_threads;
	} client_options;

	/**
		\brief Parses the command line arguments of whoClient executable and
			updates the client_options struct 
		@param argc: Number of the command line arguments
		@param argv: Array of the command lines arguments 
	*/
	__NON_NULL__(2)
	client_options parse_command_line_arguments(int argc, char *argv[]);

#endif
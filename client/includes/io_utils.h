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

	/**
		\brief Parses the query file line by line. For each line creates a thread
		in which it stores the current query. It can create up to client_options.num_threads
		threads per time.
		@param client_threads: An array to store the client_options.num_threads threads
	*/
	__NON_NULL__(1)
	void parse_query_file_and_create_threads(pthread_t *client_threads);

#endif
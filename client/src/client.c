#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../common/includes/macros.h"
#include "../includes/io_utils.h"

extern client_options options;

int main(int argc, char *argv[]) {

	options = parse_command_line_arguments(argc, argv);

	pthread_t *client_threads = __MALLOC__(options.num_threads, pthread_t);

	parse_query_file_and_create_threads(client_threads);

	return EXIT_SUCCESS;
}
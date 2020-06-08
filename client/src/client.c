#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/io_utils.h"

client_options options;

int main(int argc, char* argv[]) {

	options = parse_command_line_arguments(argc, argv);

	return EXIT_SUCCESS;
}
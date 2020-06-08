#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/io_utils.h"

#include "../../common/includes/constants.h"
#include "../../common/includes/report_utils.h"
#include "../../common/includes/string_utils.h"
#include "../../common/includes/types.h"

static inline
void __usage(void) {
  fprintf(stderr,
					"You must provide the following arguments:\n"
					"\t-q <query_file>\n"
					"\t-w <num_threads>\n"
					"\t-sp <server_port>\n"
					"\t-sip <server_ip>\n");
  exit(EXIT_SUCCESS);
}

client_options parse_command_line_arguments(int argc, char *argv[]) {
	if (argc < 9) __usage();
	client_options options = {0};
	int option;
	while (true) {
		int option_index;
		option = getopt_long_only(argc, argv, "q:w:s:i:", options_spec, &option_index);
		if (option == -1) break;
		switch (option) {
			case 'q': {
				options.query_file = strdup(optarg);
				break;
			}
			case 'w': {
				uint64_t value;
				if (!str_to_uint64(optarg, &value)) {
						die("There was an error while reading the number threads."
								"Please make sure you provide a valid number of threads.");
				}
				options.num_threads = (uint64_t) value;
				break;
			}
			case 's': {
				uint64_t value;
				if (!str_to_uint64(optarg, &value)) {
						die("There was an error while reading the server port number."
								"Please make sure you provide a valid server port number.");
				}
				options.server_port_number = (uint16_t) value;
				break;
			}
			case 'i': {
				options.server_ip = strdup(optarg);
				break;
			}
			case 'h': {
				__usage();
			}
			case '?': {
				break;
			}
			default: {
				abort();
			}
		}
	}
	return options;
}
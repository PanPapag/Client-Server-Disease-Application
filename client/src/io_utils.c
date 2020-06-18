#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../common/includes/file_utils.h"
#include "../../common/includes/string_utils.h"
#include "../../common/includes/report_utils.h"

#include "../includes/io_utils.h"

client_options options;

static inline
void __usage(void) {
  fprintf(stderr,
					"You must provide the following arguments:\n"
					"\t-q <Query File>\n"
					"\t-w <Number of Threads>\n"
					"\t-sp <Server Port>\n"
					"\t-sip <Server IP>\n");
  exit(EXIT_SUCCESS);
}

void parse_command_line_arguments(int argc, char *argv[]) {
	if (argc < 9) __usage();
	int option;
	while (1) {
		int option_index;
		option = getopt_long_only(argc, argv, "q:w:s:i:", options_spec, &option_index);
		if (option == -1) break;
		switch (option) {
			case 'q': {
				options.query_file = strdup(optarg);
        if (!file_exists(options.query_file)) {
          die("Query file <%s> does not exists. Exiting...", options.query_file);
        }
				break;
			}
			case 'w': {
				uint64_t value;
				if (!str_to_uint64(optarg, &value)) {
						die("Invalid <Number of Threads> parameter. Exiting...");
				}
				options.num_threads = (uint64_t) value;
				break;
			}
			case 's': {
				uint64_t value;
				if (!str_to_uint64(optarg, &value)) {
						die("Invalid <Server Port> parameter. Exiting...");
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
}

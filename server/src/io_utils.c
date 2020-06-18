#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../common/includes/constants.h"
#include "../../common/includes/file_utils.h"
#include "../../common/includes/report_utils.h"
#include "../../common/includes/string_utils.h"
#include "../../common/includes/types.h"

#include "../includes/io_utils.h"

server_options options;

static inline
void __usage() {
    fprintf(stderr,
            "You must provide the following arguments:\n"
            "\t-q <Query Port>\n"
            "\t-s <Statistics Port>\n"
            "\t-w <Number of Threads>\n"
            "\t-b <Buffer Size>\n");
    exit(EXIT_FAILURE);
}

void parse_command_line_arguments(int argc, char *argv[]) {
  if (argc < 9) __usage();
  int option;
  while (1) {
    int option_index;
    option = getopt_long_only(argc, argv, "q:s:w:b:", options_spec, &option_index);
    if (option == -1) break;
    switch (option) {
      case 'q': {
        uint64_t value;
        if (!str_to_uint64(optarg, &value)) {
            die("Invalid <Query Port> parameter. Exiting...");
        }
        options.query_port_number = (uint16_t) value;
        break;
      }
      case 's': {
        uint64_t value;
        if (!str_to_uint64(optarg, &value)) {
            die("Invalid <Statistics Port> parameter. Exiting...");
        }
        options.statistics_port_number = (uint16_t) value;
        break;
      }
      case 'w': {
        uint64_t value;
        if (!str_to_uint64(optarg, &value)) {
          die("Invalid <Number of Threads> parameter. Exiting...");
        }
        options.num_threads = (uint16_t) value;
        break;
      }
      case 'b': {
        uint64_t value;
        if (!str_to_uint64(optarg, &value)) {
          die("Invalid <Buffer Size> parameter. Exiting...");
        }
        options.buffer_size = (uint16_t) value;
        break;
      }
      case 'h':
        __usage();
      case '?':
        break;
      default:
        abort();
    }
  }
}

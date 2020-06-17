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

master_options options;

static inline
void __usage() {
    fprintf(stderr,
            "You must provide the following arguments:\n"
            "\t-w <Number of Workers>\n"
            "\t-b <Buffer Size>\n"
            "\t-s <Server IP>\n"
  					"\t-p <Server Port>\n"
            "\t-i <Input Directory>\n");
    exit(EXIT_FAILURE);
}

void parse_command_line_arguments(int argc, char *argv[]) {
  if (argc < 11) __usage();
  int option;
  while (1) {
    int option_index;
    option = getopt_long_only(argc, argv, "w:b:s:p:i:", options_spec, &option_index);
    if (option == -1) break;
    switch (option) {
      case 'w': {
        uint64_t value;
        if (!str_to_uint64(optarg, &value)) {
          die("Invalid <Number of Workers> parameter. Exiting...");
        }
        options.num_workers = (uint16_t) value;
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
      case 's': {
        uint64_t value;
        if (!str_to_uint64(optarg, &value)) {
            die("Invalid <Server Port> parameter. Exiting...");
        }
        options.server_port_number = (uint16_t) value;
        break;
      }
      case 'p': {
        options.server_ip = strdup(optarg);
        break;
      }
      case 'i': {
        options.input_dir = strdup(optarg);
        if (!directory_exists(options.input_dir)) {
          die("Input directory <%s> does not exists. Exiting...", options.input_dir);
        }
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

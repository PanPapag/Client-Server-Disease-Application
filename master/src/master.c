#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

// #include "../../includes/common/list.h"
// #include "../../includes/common/macros.h"
// #include "../../includes/common/io_utils.h"
// #include "../../includes/common/statistics.h"
// #include "../../includes/common/utils.h"
// #include "../../includes/aggregator/io_utils.h"
// #include "../../includes/aggregator/utils.h"

#include "../includes/io_utils.h"

extern master_options options;

int main(int argc, char *argv[]) {
  /* Parse command line arguments and update master options */
  parse_command_line_arguments(argc, argv);


  return EXIT_SUCCESS;
}

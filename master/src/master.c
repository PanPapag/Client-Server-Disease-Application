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

#include "../../common/includes/constants.h"
#include "../../common/includes/macros.h"

#include "../includes/io_utils.h"
#include "../includes/utils.h"

extern master_options options;

int main(int argc, char *argv[]) {
  parse_command_line_arguments(argc, argv);
  options.workers_pid = __MALLOC__(options.num_workers, pid_t);
  spawn_workers();
  get_and_distribute_subdirs();
  update_workers();
  clear_memory();
  return EXIT_SUCCESS;
}

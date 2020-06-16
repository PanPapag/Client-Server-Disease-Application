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

extern master_options options;

void spawn_workers(void) {
  // Convert command line argument buffer_size to string
  char buffer_size[12];
  sprintf(buffer_size, "%d", options.buffer_size);
  // Fifo array of char holds the name of named pipe via master and worker communicates
  char fifo[FIFO_NAME_SIZE];
  // Store in a variable of type pid_t the pid of each worker proccess spawned
  pid_t pid;
  for (size_t i = 0U; i < options.num_workers; ++i) {
    pid = fork();
    // Handle fork failure
    if (pid == -1) {
      perror("Failed to fork!");
      exit(EXIT_FAILURE);
    }
    if (pid == 0) {
      // Create named pipe in which parent writes and child reads
      sprintf(fifo, "fifo_%d", getpid());
      int ret_val = mkfifo(fifo, 0666);
      printf("%s\n",fifo);
      if ((ret_val == -1) && (errno != EEXIST)) {
         perror("Creating Fifo Failed");
         exit(1);
      }
      // Start worker executable passing its arguments
      execl("./whoClient", "whoClient", (char *) NULL);
      perror("execl() Execution Failed");
      exit(EXIT_FAILURE);
    } else {
      // Parent just stores in master_options workers_pid array the pids of workers
      options.workers_pid[i] = pid;
    }
  }
}

int main(int argc, char *argv[]) {
  parse_command_line_arguments(argc, argv);
  options.workers_pid = __MALLOC__(options.num_workers, pid_t);
  spawn_workers();
  __FREE__(options.workers_pid);
  __FREE__(options.input_dir);
  __FREE__(options.server_ip);
  return EXIT_SUCCESS;
}

#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../../common/includes/constants.h"
#include "../../common/includes/macros.h"
#include "../../common/includes/report_utils.h"

#include "../includes/io_utils.h"
#include "../includes/signals_handling.h"
#include "../includes/utils.h"

volatile sig_atomic_t worker_replaced;

master_options options;

static void signal_handlers(int signo) {
  if (signo == SIGCHLD) {
    worker_replaced = 1;
    while (1) {
      int status;
      pid_t pid = waitpid(-1, &status, WNOHANG);
      if (pid <= 0) {
        break;
      }
      /* Handle dead worker */
      char fifo[FIFO_NAME_SIZE];
      int ret_val;
      pid_t new_pid;
      // Convert command line argument buffer_size to string
      char buffer_size[12];
      sprintf(buffer_size, "%d", options.buffer_size);
      // Convert server port number to string
      char server_port_number[12];
      sprintf(server_port_number, "%d", options.server_port_number);
      /* Get position of the killed pid */
      int pos = get_worker_fd_pos(pid);
      /* Unlink old named pipes */
      sprintf(fifo, "pw_cr_%d", pid);
      unlink(fifo);
      /* Fork a new worker and update him */
      new_pid = fork();
      if (new_pid == -1) {
        perror("Failed to fork!");
        exit(EXIT_FAILURE);
      }
      if (new_pid == 0) {
        /* Create named pipe in which parent writes and child reads */
        sprintf(fifo, "fifo_%d", getpid());
        ret_val = mkfifo(fifo, 0666);
        if ((ret_val == -1) && (errno != EEXIST)) {
           perror("Creating Fifo Failed");
           exit(1);
        }
        raise(SIGSTOP);
        /* Call the new worker */
        execl("./worker", "worker", "-f", fifo, "-b", buffer_size,
              "-s", options.server_ip, "-p", server_port_number, (char *) NULL);
        perror("execl() Execution Failed");
        exit(EXIT_FAILURE);
      } else {
        /* Aggregator updates the new forked worker */
        waitpid(new_pid, NULL, WUNTRACED);
        kill(new_pid, SIGCONT);
        update_worker(new_pid, pos);
      }
    }
  }
}

void register_signals_handlers(void) {
  struct sigaction act;
  memset(&act, 0, sizeof(act));
  act.sa_handler = signal_handlers;
  sigfillset(&(act.sa_mask));
  act.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &act, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }
}

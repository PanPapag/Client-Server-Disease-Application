#ifndef __MASTER_UTILS__
  #define __MASTER_UTILS__

  #include "../../common/includes/attributes.h"
  #include "../../common/includes/list.h"

  /**
    \brief A wrapper function which call distribute_subdirs and get_subdirs
      while updating master_options workers_dir_paths
  */
  void get_and_distribute_subdirs(void);

  /**
    \brief A wrapper function which call update_worker for each worker proccess
  */
  void update_workers(void);

  /**
    \brief Create options.num_workers child proccesses and exec the worker executable
  */
  void spawn_workers(void);

  /*
    Given a worker's pid returns the position of fds in the program_parameters
    workers_fd_1 and workers_fd_2 arrays
  */
  int get_worker_fd_pos(pid_t pid);

#endif

#ifndef __MASTER_UTILS__
  #define __MASTER_UTILS__

  #include "../../common/includes/attributes.h"
  #include "../../common/includes/list.h"

  /**
    \brief Distributes the sub directories of the input dir to num_workers
    @param directories: A list of the directories we have to distribute
    return: An array which we store in each index i a string with
      the names of directories seperated with space for the worker i
  */
  __NON_NULL__(1)
  char** distribute_subdirs(list_ptr directories);

  /**
    \brief A wrapper function which call distribute_subdirs and get_subdirs
      while updating master_options workers_dir_paths
  */
  void get_and_distribute_subdirs(void);

  /**
   \breif Sends to a specific worker the directories he has to parse
   @param pid: The worker's pid
   @param pos: The position of workers info in the master_options array
  */
  void update_worker(pid_t pid, int pos);

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

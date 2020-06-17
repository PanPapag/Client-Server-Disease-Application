#ifndef __WORKER_UTILS__
  #define __WORKER_UTILS__

  #include "../../common/includes/hash_table.h"
  #include "../../common/includes/list.h"

  typedef struct worker_structures {
    hash_table_ptr country_ht;
    hash_table_ptr disease_ht;
    hash_table_ptr file_paths_ht;
    hash_table_ptr patient_record_ht;
    list_ptr countries_names;
    list_ptr diseases_names;
    list_ptr files_statistics;
  } worker_structures;

  extern worker_structures structures;

  /**
    \brief Creates all data structures needed by worker to operate successfully
  */
  void create_global_data_structures(void);

  /**
    \brief Wrapper function which reads from fifo parse directories and updates
      global data structures
  */
  void parse_dirs_and_update_global_data_structures(void);

#endif

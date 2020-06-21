#ifndef __WORKER_UTILS__
  #define __WORKER_UTILS__

  #include "../../common/includes/hash_table.h"
  #include "../../common/includes/ipv4_socket.h"
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

  extern ipv4_socket server_socket;
  extern ipv4_socket worker_socket;

  /**
    \brief Creates all data structures needed by worker to operate successfully
  */
  void create_global_data_structures(void);

  /**
    \brief Wrapper function which reads from fifo parse directories and updates
      global data structures
  */
  void parse_dirs_and_update_global_data_structures(void);

  /**
    \brief In order to be able to get queries from the server and send the results
     back to him each worker plays the role of a server in which our actual server
     connects by the time a query has sent
  */
  void setup_worker_socket(void);

  /**
    \brief Sends hostname and port number to server in order him to connect
  */
  void send_hostname_and_port_to_server(void);

  /**
    \brief Creates and connects via statistics socket to the server to send
      all file statistics
  */
  void send_statistics_to_server(void);

  /**
    \brief Receives queries from server and sends back the result
  */
  void start_worker_as_server(void);

#endif

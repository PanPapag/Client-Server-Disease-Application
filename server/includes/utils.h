#ifndef __SERVER_UTILS__
	#define __SERVER_UTILS__

	typedef struct worker_connect_info {
		char *hostname;
    uint16_t port_number;
  } worker_connect_info;

	typedef worker_connect_info* workers_connect_info_ptr;
	
	/**
		\brief Creates all data structures needed by server to operate successfully
	*/
	void create_global_data_structures(void);

  /**
    \brief It creates the main thread which accepts socket fd and place them to
      pool, while createing numThreads waiting to consume socket fds from pool
      and execute correspondent action
  */
  void create_threads_and_start_server(void);

	/**
		\brief For every socket needed - Creates, binds and listens
	*/
	void setup_server_connections(void);

#endif

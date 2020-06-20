#ifndef __SERVER_UTILS__
	#define __SERVER_UTILS__

	#include "../../common/includes/list.h"

	#include "pool.h"

	typedef struct server_structures {
  	pool_t pool;
    list_ptr workers_port_number;
  } server_structures;

  extern server_structures structures;

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

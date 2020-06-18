#ifndef __SERVER_UTILS__
	#define __SERVER_UTILS__

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

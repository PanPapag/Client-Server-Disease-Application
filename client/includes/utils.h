#ifndef __CLIENT_UTILS__
	#define __CLIENT_UTILS__

	#include "../../common/includes/attributes.h"

  /**
    \brief Parses a query file while creating num_threads threads to send the
      queries to the server
		@param client_threads: An array of type pthread_t to store num_threads threads
  */
	__NON_NULL__(1)
  void parse_query_file_and_create_threads(pthread_t *client_threads);

#endif

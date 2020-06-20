#ifndef __SERVER_POOL__
	#define __SERVER_POOL__

  #include <pthread.h>

	#include "../../common/includes/ipv4_socket.h"

  typedef struct pool {
    ipv4_socket *data;
    int size;
    int start;
    int end;
    int count;
  } pool_t;

  typedef pool_t* pool_ptr;

	extern pool_t pool;

  extern pthread_cond_t cond_nonempty;
  extern pthread_cond_t cond_nonfull;
  extern pthread_mutex_t mtx;

  void initialize_pool(pool_ptr pool, int pool_size);
  void destroy_pool(pool_ptr pool);

  void place_in_pool(pool_ptr pool, ipv4_socket data);
  ipv4_socket obtain_from_pool(pool_ptr pool);

#endif

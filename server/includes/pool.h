#ifndef __SERVER_POOL__
	#define __SERVER_POLL__

  #include <pthread.h>

  typedef struct pool {
    int *data;
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

  void place_in_pool(pool_ptr pool, int data);
  int obtain_from_pool(pool_ptr pool);

#endif

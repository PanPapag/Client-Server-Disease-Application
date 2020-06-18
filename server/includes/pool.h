#ifndef __SERVER_POOL__
	#define __SERVER_POLL__

  #include <pthread.h>

  typedef struct pool {
    int *data;
    int size;
    int start;
    int end;
    int count;
  } pool;

  typedef pool* pool_ptr;

  extern pthread_cond_t cond_nonempty;
  extern pthread_cond_t cond_nonfull;
  extern pthread_mutex_t mtx;

  void initialize(pool_ptr pool, int pool_size);
  void destroy(pool_ptr pool);

  void place(pool_ptr pool, int data);
  int obtain(pool_ptr pool);

#endif

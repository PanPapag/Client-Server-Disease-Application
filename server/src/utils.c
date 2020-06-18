#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../includes/io_utils.h"
#include "../includes/pool.h"
#include "../includes/utils.h"

server_options options;

pool_t pool;

pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
pthread_mutex_t mtx;

static void* __accept_connections(void *args) {
  while (1) {
    place_in_pool(&pool, rand_num);
    printf("producer: %d\n", rand_num);
    pthread_cond_signal(&cond_nonempty);
    usleep(0);
  }
  pthread_exit(0);
}

static void* __get_connections(void *args) {
  while (1) {
    printf("consumer %d: %d\n", pthread_self(), obtain_from_pool(&pool));
    pthread_cond_signal(&cond_nonfull);
    usleep(500000);
  }
  pthread_exit(0);
}

void create_threads_and_start_server(void) {
  pthread_t producer;
  pthread_t consumer[options.num_threads];
  pthread_mutex_init(&mtx, 0);
  pthread_cond_init(&cond_nonempty, 0);
  pthread_cond_init(&cond_nonfull, 0);
  for (size_t i = 0U; i < options.num_threads; ++i) {
    pthread_create(&consumer[i], 0, __get_connections, 0);
  }
  pthread_create(&producer, 0, __accept_connections, 0);
  pthread_join(producer, 0);
  for (size_t i = 0U; i < options.num_threads; ++i) {
    pthread_join(consumer[i], 0);
  }
  pthread_cond_destroy(&cond_nonempty);
  pthread_cond_destroy(&cond_nonfull);
  pthread_mutex_destroy(&mtx);
}

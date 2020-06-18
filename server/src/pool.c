#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "../../common/includes/ipv4_socket.h"
#include "../../common/includes/macros.h"
#include "../../common/includes/report_utils.h"

#include "../includes/pool.h"

pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
pthread_mutex_t mtx;

void initialize_pool(pool_ptr pool, int pool_size) {
  pool->data = __MALLOC__(pool_size, ipv4_socket);
  if (pool->data == NULL) {
    die("Could not allocate memory for pool.");
  }
  pool->size = pool_size;
  pool->start = 0;
  pool->end = -1;
  pool->count = 0;
}

void destroy_pool(pool_ptr pool) {
  __FREE__(pool->data);
}

void place_in_pool(pool_ptr pool, ipv4_socket data) {
  pthread_mutex_lock(&mtx);
  while (pool->count >= pool->size) {
    pthread_cond_wait(&cond_nonfull, &mtx);
  }
  pool->end = (pool->end + 1) % pool->size;
  pool->data[pool->end] = data;
  pool->count++;
  pthread_mutex_unlock(&mtx);
}

ipv4_socket obtain_from_pool(pool_ptr pool) {
  ipv4_socket data = {0};
  pthread_mutex_lock(&mtx);
  while (pool->count <= 0) {
    pthread_cond_wait(&cond_nonempty, &mtx);
  }
  data = pool->data[pool->start];
  pool->start = (pool->start + 1) % pool->size;
  pool->count--;
  pthread_mutex_unlock(&mtx);
  return data;
}

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../common/includes/ipv4_socket.h"
#include "../../common/includes/message.h"
#include "../../common/includes/report_utils.h"
#include "../../common/includes/statistics.h"

#include "../includes/io_utils.h"
#include "../includes/pool.h"
#include "../includes/utils.h"

ipv4_socket server_statistics_socket;
ipv4_socket server_query_socket;

server_options options;

pool_t pool;

pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
pthread_mutex_t mtx;

static void __setup_statistics_socket(void) {
  if (ipv4_socket_create(options.statistics_port_number, IPV4_ANY_ADDRESS,
                         &server_statistics_socket) < 0) {
    die("Could not create address for server to statistics socket!");
  }
  if (ipv4_socket_bind(&server_statistics_socket) < 0) {
    die("Could not bind server address to statistics socket!");
  }
  if (ipv4_socket_listen(&server_statistics_socket) < 0) {
    die("Could not register server address for listening to statistics socket!");
  }
}

static void __setup_query_socket(void) {
  if (ipv4_socket_create(options.query_port_number, IPV4_ANY_ADDRESS,
                         &server_query_socket) < 0) {
    die("Could not create address for server to query socket!");
  }
  if (ipv4_socket_bind(&server_query_socket) < 0) {
    die("Could not bind server address to query socket!");
  }
  if (ipv4_socket_listen(&server_query_socket) < 0) {
    die("Could not register server address for listening to query socket!");
  }
}

void setup_server_connections(void) {
  __setup_statistics_socket();
  __setup_query_socket();
}

static void __serve_num_statistics(int num_statistics, ipv4_socket connected_socket) {
  for (size_t i = 0U; i < num_statistics; ++i) {
    message message = ipv4_socket_get_message(&connected_socket);
    if (message.header.id != STATISTICS) {
      report_warning("Unknown format instead of statistics has been read!");
    }
    serialized_statistics_entry_print((char*) message.data);
  }
}

static void __handle_message(message message, ipv4_socket connected_socket) {
  switch (message.header.id) {
    case QUERY: {
      printf("consumer - query: %s\n",(char*) message.data);
      break;
    }
    case NUM_STATISTICS: {
      __serve_num_statistics(atoi((char*) message.data), connected_socket);
      break;
    }
    default:
      abort();
  }
}

static void* __accept_connections(void *args) {
  ipv4_socket connected_socket;
  while (1) {
    ipv4_socket_accept(&server_query_socket, &connected_socket);
    place_in_pool(&pool, connected_socket);
    // printf("producer: %d\n", connected_socket.socket_fd);
    pthread_cond_signal(&cond_nonempty);
  }
  pthread_exit(0);
}

static void* __get_connections(void *args) {
  ipv4_socket connected_socket;
  message message;
  while (1) {
    connected_socket = obtain_from_pool(&pool);
    message = ipv4_socket_get_message(&connected_socket);
    __handle_message(message, connected_socket);
    pthread_cond_signal(&cond_nonfull);
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

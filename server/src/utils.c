#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/select.h>

#include "../../common/includes/constants.h"
#include "../../common/includes/ipv4_socket.h"
#include "../../common/includes/list.h"
#include "../../common/includes/message.h"
#include "../../common/includes/report_utils.h"
#include "../../common/includes/statistics.h"
#include "../../common/includes/utils.h"

#include "../includes/io_utils.h"
#include "../includes/pool.h"
#include "../includes/utils.h"

fd_set server_sockets_set;

ipv4_socket server_statistics_socket;
ipv4_socket server_query_socket;

server_options options;

server_structures structures;

pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
pthread_mutex_t output_mtx;
pthread_mutex_t mtx;

void create_global_data_structures(void) {
  initialize_pool(&structures.pool, options.buffer_size);
  structures.workers_port_number = list_create(int*, int_compare, int_print, int_destroy);
}

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
  pthread_mutex_init(&output_mtx, 0);
  for (size_t i = 0U; i < num_statistics; ++i) {
    message message = ipv4_socket_get_message(&connected_socket);
    if (message.header.id != STATISTICS) {
      report_warning("Unknown format instead of statistics has been read!");
    }
    serialized_statistics_entry_print((char*) message.data);
  }
  pthread_mutex_destroy(&output_mtx);
}

static void __serve_hostname_and_port(char *hostname_and_port) {
  char *hostname;
  int *port_number;
  // get the first token (aka hostname)
  options.workers_ip_address = strdup(strtok(hostname_and_port, SPACE));
  // get the second token (aka port number)
  port_number = int_create(atoi(strtok(NULL, SPACE)));
  // Save worker's port number to list
  list_push_back(&structures.workers_port_number, port_number);
}

static void __serve_query(char *query, ipv4_socket connected_socket) {
  ipv4_socket worker_socket;
  message message;
  for (size_t i = 1U; i < list_size(structures.workers_port_number); ++i) {
    list_node_ptr list_node = list_get(structures.workers_port_number, i);
    int port_number = (*(int*) list_node->data_);
    // Create connections with worker i
    if (ipv4_socket_create_and_connect(options.workers_ip_address, port_number, &worker_socket)) {
      // Send query to worker i
      message = message_create(query, QUERY);
      if (!ipv4_socket_send_message(&worker_socket, message)) {
        report_warning("Message <%s> could not be sent to worker!", (char*) message.data);
      }
      message_destroy(&message);
      // Get result from the worker
      message = ipv4_socket_get_message(&worker_socket);
      printf("%s\n",(char*) message.data);
      // // Send result back to client
      if (!ipv4_socket_send_message(&connected_socket, message)) {
        report_warning("Message <%s> could not be sent to client!", (char*) message.data);
      }
    }
  }
}

static void __handle_message(message message, ipv4_socket connected_socket) {
  switch (message.header.id) {
    case QUERY: {
      __serve_query((char*) message.data, connected_socket);
      break;
    }
    case HOSTNAME_AND_PORT: {
      __serve_hostname_and_port((char*) message.data);
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

static void __reset_server_sockets_set(void) {
  FD_ZERO(&server_sockets_set);
  FD_SET(server_statistics_socket.socket_fd, &server_sockets_set);
  FD_SET(server_query_socket.socket_fd, &server_sockets_set);
}

static void __get_selected_socket(ipv4_socket_ptr selected_socket) {
   if (FD_ISSET(server_statistics_socket.socket_fd, &server_sockets_set)) {
     *selected_socket = server_statistics_socket;
   } else if (FD_ISSET(server_query_socket.socket_fd, &server_sockets_set)) {
     *selected_socket = server_query_socket;
   }
}

static void* __accept_connections(void *args) {
  ipv4_socket selected_socket;
  ipv4_socket connected_socket;
  while (1) {
    __reset_server_sockets_set();
    int retval = select(FD_SETSIZE, &server_sockets_set, NULL, NULL, NULL);
    if (retval) {
      __get_selected_socket(&selected_socket);
      ipv4_socket_accept(&selected_socket, &connected_socket);
      place_in_pool(&structures.pool, connected_socket);
      pthread_cond_signal(&cond_nonempty);
    }
  }
  pthread_exit(0);
}

static void* __get_connections(void *args) {
  ipv4_socket connected_socket;
  message message;
  while (1) {
    connected_socket = obtain_from_pool(&structures.pool);
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

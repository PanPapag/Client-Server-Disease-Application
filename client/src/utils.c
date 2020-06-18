#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../common/includes/constants.h"
#include "../../common/includes/file_utils.h"
#include "../../common/includes/ipv4_socket.h"
#include "../../common/includes/message.h"
#include "../../common/includes/report_utils.h"
#include "../../common/includes/request.h"
#include "../../common/includes/string_utils.h"
#include "../../common/includes/types.h"

#include "../includes/io_utils.h"
#include "../includes/utils.h"

client_options options;

static pthread_barrier_t barrier;

static inline
void* __client_threads_function(void *fl) {
	char *query = (char*) fl;
  pthread_barrier_wait(&barrier);
	// Establish connection with server
	ipv4_socket client_socket;
	if (ipv4_socket_create_and_connect(options.server_ip, options.server_port_number, &client_socket)) {
		printf("Established! - %s\n", query);
		message message = create_query_message(query);
		printf("%c ----- %s\n", message.header.id,  (char*) message.data);
	}
}

void parse_query_file_and_create_threads(pthread_t *client_threads) {
  char buffer[options.num_threads][MAX_BUFFER_SIZE];
  /* Get the number of lines of the query file */
  int num_queries;
  get_file_lines(options.query_file, &num_queries);
  /*
    As the description mentions: "When all the threads are created, that is,
    we have one thread for each command of the file, then the threads should
    start all together to try to connect to whoServer and send their command.

    Given the statement above, we have to take care of 2 cases.
    1. num_threads >= num_queries: We create just num_queries threads and nothing more
    3. num_threads < num_queries: This case is the most complex one. To tackle
       this problem, when num_threads are fewer than the number of queries, we
       are going to create (num_queries) div (num_threads) times exactly num_threads
       threads to serve num_threads queries per time. The last time we are going
       to create just (num_queries) mod (num_threads) threads to serve the remaining
       queries.
  */
  /* Open file for read only - handles binary fille too */
  FILE *fp = fopen(options.query_file, "rb+");
  /* Seperate the 2 cases */
  if (options.num_threads >= num_queries) {
    /* Initialize pthread barrier */
    pthread_barrier_init(&barrier, NULL, num_queries);
    for (size_t i = 0U; i < num_queries; ++i) {
      /* Read query and store it to the buffer */
      fgets(buffer[i], MAX_BUFFER_SIZE, fp);
      /* Discard '\n' that fgets() stores */
      buffer[i][strlen(buffer[i]) - 1] = '\0';
      /* Create thread */
      pthread_create(&client_threads[i], NULL, __client_threads_function, (void *)buffer[i]);
    }
    /* Wait all threads to finish and destroy barrier */
    for (size_t i = 0U; i < num_queries; ++i) {
      pthread_join(client_threads[i], NULL);
    }
		pthread_barrier_destroy(&barrier);
  } else {
    size_t integral_comm = num_queries / options.num_threads;
    size_t remaining_comm = num_queries % options.num_threads;
    /* Complete the integral communications between clients and server */
    for (size_t i = 0U; i < integral_comm; ++i) {
      /* Initialize pthread barrier to wait num_threads threads */
      pthread_barrier_init(&barrier, NULL, options.num_threads );
      for (size_t j = 0U; j < options.num_threads; ++j) {
        /* Read query and store it to the buffer */
        fgets(buffer[j], MAX_BUFFER_SIZE, fp);
        /* Discard '\n' that fgets() stores */
        buffer[j][strlen(buffer[j]) - 1] = '\0';
        /* Create thread */
        pthread_create(&client_threads[j], NULL, __client_threads_function, (void *)buffer[j]);
      }
      /* Wait all threads to finish and destroy barrier */
      for (size_t j = 0U; j < options.num_threads; ++j) {
        pthread_join(client_threads[j], NULL);
      }
      pthread_barrier_destroy(&barrier);
    }
    /* Complete the remaining ones */
    if (remaining_comm) {
      /* Initialize pthread barrier to wait remaining_comm threads */
      pthread_barrier_init(&barrier, NULL, remaining_comm);
      for (size_t j = 0U; j < remaining_comm; ++j) {
        /* Read query and store it to the buffer */
        fgets(buffer[j], MAX_BUFFER_SIZE, fp);
        /* Discard '\n' that fgets() stores */
        buffer[j][strlen(buffer[j]) - 1] = '\0';
        /* Create thread */
        pthread_create(&client_threads[j], NULL, __client_threads_function, (void *)buffer[j]);
      }
      /* Wait all threads to finish and destroy barrier */
      for (size_t j = 0U; j < remaining_comm; ++j) {
        pthread_join(client_threads[j], NULL);
      }
      pthread_barrier_destroy(&barrier);
    }
  }
  /* Close file pointer */
  fclose(fp);
}

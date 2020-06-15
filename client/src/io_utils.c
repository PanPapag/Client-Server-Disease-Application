#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../includes/io_utils.h"

#include "../../common/includes/constants.h"
#include "../../common/includes/file_utils.h"
#include "../../common/includes/report_utils.h"
#include "../../common/includes/string_utils.h"
#include "../../common/includes/types.h"

client_options options;

static pthread_barrier_t barrier;

static inline
void __usage(void) {
  fprintf(stderr,
					"You must provide the following arguments:\n"
					"\t-q <query_file>\n"
					"\t-w <num_threads>\n"
					"\t-sp <server_port>\n"
					"\t-sip <server_ip>\n");
  exit(EXIT_SUCCESS);
}

client_options parse_command_line_arguments(int argc, char *argv[]) {
	if (argc < 9) __usage();
	client_options options = {0};
	int option;
	while (true) {
		int option_index;
		option = getopt_long_only(argc, argv, "q:w:s:i:", options_spec, &option_index);
		if (option == -1) break;
		switch (option) {
			case 'q': {
				options.query_file = strdup(optarg);
        // TODO check if exists
				break;
			}
			case 'w': {
				uint64_t value;
				if (!str_to_uint64(optarg, &value)) {
						die("There was an error while reading the number threads."
								"Please make sure you provide a valid number of threads.");
				}
				options.num_threads = (uint64_t) value;
				break;
			}
			case 's': {
				uint64_t value;
				if (!str_to_uint64(optarg, &value)) {
						die("There was an error while reading the server port number."
								"Please make sure you provide a valid server port number.");
				}
				options.server_port_number = (uint16_t) value;
				break;
			}
			case 'i': {
				options.server_ip = strdup(optarg);
				break;
			}
			case 'h': {
				__usage();
			}
			case '?': {
				break;
			}
			default: {
				abort();
			}
		}
	}
	return options;
}

static inline
void *__client_threads_function(void *fl) {
	char *query = (char*) fl;
	printf("Waiting... [%s]\n", query);

  pthread_barrier_wait(&barrier);

	printf("Sending... [%s]\n", query);
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
    printf("CASE 2\n");
  }
  /* Close file pointer */
  fclose(fp);
}

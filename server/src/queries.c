#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <wordexp.h>

#include "../../common/includes/constants.h"
#include "../../common/includes/ipv4_socket.h"
#include "../../common/includes/list.h"
#include "../../common/includes/report_utils.h"

#include "../includes/io_utils.h"
#include "../includes/queries.h"
#include "../includes/utils.h"

pthread_mutex_t output_mtx;

server_options options;

server_structures structures;

void serve_num_patient(char *query, ipv4_socket connected_socket) {
  int result = 0;
  ipv4_socket worker_socket;
  message message;
  char result_buf[12];
  for (size_t i = 1U; i <= list_size(structures.workers_port_number); ++i) {
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
      if (message.header.id != RESPONSE) {
        report_warning("Unknown format instead of response has been read!");
      } else {
        if (strcmp((char*) message.data, NO_RESPONSE)) {
          result += atoi((char*) message.data);
        }
      }
      message_destroy(&message);
    }
  }
  // Update client that is going to receive a single message
  message = message_create("1", RESPONSE);
  if (!ipv4_socket_send_message(&connected_socket, message)) {
    report_warning("Message <%s> could not be sent to client!", (char*) message.data);
  }
  message_destroy(&message);
  // Send result back to client
  sprintf(result_buf, "%d", result);
  message = message_create(result_buf, RESPONSE);
  if (!ipv4_socket_send_message(&connected_socket, message)) {
    report_warning("Message <%s> could not be sent to client!", (char*) message.data);
  }
  message_destroy(&message);
  // Print query and response to server
  pthread_mutex_lock(&output_mtx);
  printf("Q: %s\n", query);
  printf("R: %d\n", result);
  pthread_mutex_unlock(&output_mtx);
}

void serve_topk_age_ranges(char *query, ipv4_socket connected_socket) {
  message message;
  // Update client that is going to receive a single message
  message = message_create("1", RESPONSE);
  if (!ipv4_socket_send_message(&connected_socket, message)) {
    report_warning("Message <%s> could not be sent to client!", (char*) message.data);
  }
  message_destroy(&message);
  // Send result back to client
  message = message_create("NOT YET", RESPONSE);
  if (!ipv4_socket_send_message(&connected_socket, message)) {
    report_warning("Message <%s> could not be sent to client!", (char*) message.data);
  }
  message_destroy(&message);
  // Print query and response to server
  pthread_mutex_lock(&output_mtx);
  printf("Q: %s\n", query);
  printf("R: NOT YET\n");
  pthread_mutex_unlock(&output_mtx);
}

void serve_search_patient_record(char *query, ipv4_socket connected_socket) {
  bool found_result = false;
  ipv4_socket worker_socket;
  message message;
  char no_messages_buf[12];
  // Update client to be ready to recieve num_workers messages
  sprintf(no_messages_buf, "%ld", list_size(structures.workers_port_number));
  message = message_create(no_messages_buf, RESPONSE);
  if (!ipv4_socket_send_message(&connected_socket, message)) {
    report_warning("Message <%s> could not be sent to client!", (char*) message.data);
  }
  message_destroy(&message);
  // Send message from each worker
  pthread_mutex_lock(&output_mtx);
  printf("Q: %s\n", query);
  for (size_t i = 1U; i <= list_size(structures.workers_port_number); ++i) {
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
      if (message.header.id != RESPONSE) {
        report_warning("Unknown format instead of response has been read!");
      } else {
        if (strcmp((char*) message.data, NO_RESPONSE)) {
          found_result = true;
          printf("%s\n", (char*) message.data);
        }
        // Send result back to client
        if (!ipv4_socket_send_message(&connected_socket, message)) {
          report_warning("Message <%s> could not be sent to client!", (char*) message.data);
        }
      }
      message_destroy(&message);
    }
  }
  if (!found_result) {
    printf("R: -\n");
  }
  pthread_mutex_unlock(&output_mtx);
}

void handle_queries(char *query, ipv4_socket connected_socket) {
  wordexp_t p;
  // Use API POSIX to extract arguments
  wordexp(query, &p, 0);
  // Call correspoding command function
  if (!strcmp(p.we_wordv[0], "/diseaseFrequency")) {
    SERVE_DISEASE_FREQUENCY(query, connected_socket);
  } else if (!strcmp(p.we_wordv[0], "/topk-AgeRanges")) {
    SERVE_TOPK_AGE_RANGES(query, connected_socket);
  } else if (!strcmp(p.we_wordv[0], "/searchPatientRecord")) {
    SERVE_SEACH_PATIENT_RECORD(query, connected_socket);
  } else if (!strcmp(p.we_wordv[0], "/numPatientAdmissions")) {
    SERVE_NUM_PATIENT_ADMISSIONS(query, connected_socket);
  } else if (!strcmp(p.we_wordv[0], "/numPatientDischarges")) {
    SERVE_NUM_PATIENT_DISCHARGES(query, connected_socket);
  }
  // Free wordexp object
  wordfree(&p);
}

#include <stdio.h>
#include <wordexp.h>

#include "../../common/includes/constants.h"
#include "../../common/includes/ipv4_socket.h"
#include "../../common/includes/list.h"
#include "../../common/includes/report_utils.h"

#include "../includes/io_utils.h"
#include "../includes/queries.h"
#include "../includes/utils.h"

server_options options;

server_structures structures;

void serve_num_patient(char *query, ipv4_socket connected_socket) {
  int result = 0;
  ipv4_socket worker_socket;
  message message;
  char result_buf[12];
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
  // // Update client to receive a single message
  // char no_messages[12];
  // sprintf(no_messages, "%d", 1);
  // message = message_create(no_messages, RESPONSE);
  // if (!ipv4_socket_send_message(&connected_socket, message)) {
  //   report_warning("Message <%s> could not be sent to client!", (char*) message.data);
  // }
  // message_destroy(&message);
  // Send result back to client
  sprintf(result_buf, "%d", result);
  message = message_create(result_buf, RESPONSE);
  if (!ipv4_socket_send_message(&connected_socket, message)) {
    report_warning("Message <%s> could not be sent to client!", (char*) message.data);
  }
  message_destroy(&message);
}

// void serve_search_patient_record(char *query, ipv4_socket connected_socket) {
//   ipv4_socket worker_socket;
//   message message;
//   // Update client to receive list_size(structures.workers_port_number) messages
//   char no_messages[12];
//   sprintf(no_messages, "%ld", list_size(structures.workers_port_number));
//   message = message_create(no_messages, RESPONSE);
//   if (!ipv4_socket_send_message(&connected_socket, message)) {
//     report_warning("Message <%s> could not be sent to client!", (char*) message.data);
//   }
//   message_destroy(&message);
//   // Send message from each worker
//   for (size_t i = 1U; i < list_size(structures.workers_port_number); ++i) {
//     list_node_ptr list_node = list_get(structures.workers_port_number, i);
//     int port_number = (*(int*) list_node->data_);
//     // Create connections with worker i
//     if (ipv4_socket_create_and_connect(options.workers_ip_address, port_number, &worker_socket)) {
//       // Send query to worker i
//       message = message_create(query, QUERY);
//       if (!ipv4_socket_send_message(&worker_socket, message)) {
//         report_warning("Message <%s> could not be sent to worker!", (char*) message.data);
//       }
//       message_destroy(&message);
//       // Get result from the worker
//       message = ipv4_socket_get_message(&worker_socket);
//       if (message.header.id != RESPONSE) {
//         report_warning("Unknown format instead of response has been read!");
//       } else {
//         if (!ipv4_socket_send_message(&connected_socket, message)) {
//           report_warning("Message <%s> could not be sent to client!", (char*) message.data);
//         }
//         printf("%s\n", query);
//         printf("%s\n", (char*) message.data);
//       }
//       message_destroy(&message);
//     }
//   }
// }

void handle_queries(char *query, ipv4_socket connected_socket) {
  wordexp_t p;
  // Use API POSIX to extract arguments
  wordexp(query, &p, 0);
  // Call correspoding command function
  if (!strcmp(p.we_wordv[0], "/diseaseFrequency")) {
    SERVE_DISEASE_FREQUENCY(query, connected_socket);
  } else if (!strcmp(p.we_wordv[0], "/topk-AgeRanges")) {
    printf("%s\n", p.we_wordv[0]);
  } else if (!strcmp(p.we_wordv[0], "/searchPatientRecord")) {
    printf("%s\n", p.we_wordv[0]);
    // SERVE_SEACH_PATIENT_RECORD(query, connected_socket);
  } else if (!strcmp(p.we_wordv[0], "/numPatientAdmissions")) {
    SERVE_NUM_PATIENT_ADMISSIONS(query, connected_socket);
  } else if (!strcmp(p.we_wordv[0], "/numPatientDischarges")) {
    SERVE_NUM_PATIENT_DISCHARGES(query, connected_socket);
  }
  // Free wordexp object
  wordfree(&p);
}

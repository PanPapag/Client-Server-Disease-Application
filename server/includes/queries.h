#ifndef __SERVER_QUERIES__
	#define __SERVER_QUERIES__

  #include "../../common/includes/ipv4_socket.h"

  void handle_queries(char *query, ipv4_socket connected_socket);

  void serve_num_patient_admissions(char *query, ipv4_socket connected_socket);

#endif

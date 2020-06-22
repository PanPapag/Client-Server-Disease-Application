#ifndef __SERVER_QUERIES__
	#define __SERVER_QUERIES__

  #include "../../common/includes/ipv4_socket.h"

	#define SERVE_DISEASE_FREQUENCY(query, socket) serve_num_patient(query, socket)
	#define SERVE_TOPK_AGE_RANGES(query, socket) serve_topk_age_ranges(query, socket)
	#define SERVE_SEACH_PATIENT_RECORD(query, socket) serve_search_patient_record(query, socket)
	#define SERVE_NUM_PATIENT_ADMISSIONS(query, socket) serve_num_patient(query, socket)
	#define SERVE_NUM_PATIENT_DISCHARGES(query, socket) serve_num_patient(query, socket)

  void handle_queries(char *query, ipv4_socket connected_socket);

  void serve_num_patient(char *query, ipv4_socket connected_socket);

	void serve_search_patient_record(char *query, ipv4_socket connected_socket);

#endif

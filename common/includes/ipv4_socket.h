#ifndef __COMMON_IPV4_SOCKET__
  #define __COMMON_IPV4_SOCKET__

  #include <netdb.h>
  #include <netinet/in.h>

  #include <stdbool.h>
  #include <stdint.h>

  #include "attributes.h"
  #include "types.h"
  #include "message.h"

  #define IPV4_SOCKET_BACKLOG_SIZE 5
  #define IPV4_ANY_ADDRESS (struct in_addr) {htonl(INADDR_ANY)}
  #define IPV4_ADDRESS_SIZE (sizeof(uint32_t) + sizeof(uint16_t))
  #define IPV4_BYTES 4
  #define MIN_IVP4_LENGTH 7
  #define MAX_IPV4_LENGTH 15
  #define MAX_IPV4_OCTET 255
  #define MIN_IPV4_OCTET 1

  typedef struct ipv4_socket {
    struct sockaddr_in address;
    int socket_fd;
  } ipv4_socket;

  typedef ipv4_socket* ipv4_socket_ptr;

  __NON_NULL__(3)
  int ipv4_socket_create(uint16_t port_number, struct in_addr in_address, ipv4_socket_ptr out_socket);

  __NON_NULL__(1)
  int ipv4_socket_bind(ipv4_socket_ptr socket);

  __NON_NULL__(1)
  int ipv4_socket_listen(ipv4_socket_ptr socket);

  __NON_NULL__(1)
  int ipv4_socket_accept(ipv4_socket_ptr server_socket, ipv4_socket_ptr client_socket);

  __NON_NULL__(1)
  int ipv4_socket_connect(ipv4_socket_ptr socket);

  __NON_NULL__(1)
  int ipv4_socket_get_port(ipv4_socket_ptr socket, uint16_t *port_number);

  __NON_NULL__(1, 3)
  bool ipv4_socket_create_and_connect(string ip_address, uint16_t port_number, ipv4_socket_ptr socket_out);

  __NON_NULL__(1)
  ssize_t ipv4_socket_send_message(ipv4_socket_ptr receiver, message message);

  __NON_NULL__(1)
  message ipv4_socket_get_message(ipv4_socket_ptr sender);

#endif

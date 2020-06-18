#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <strings.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <zconf.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "../includes/ipv4_socket.h"
#include "../includes/macros.h"
#include "../includes/report_utils.h"
#include "../includes/request.h"
#include "../includes/types.h"

int ipv4_socket_create(uint16_t port_number, struct in_addr in_address, ipv4_socket_ptr out_socket) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) return -1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));
  struct sockaddr_in address;
  bzero(&address, sizeof(struct sockaddr_in));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = in_address.s_addr;
  address.sin_port = htons(port_number);
  out_socket->address = address;
  out_socket->socket_fd = fd;
  return 0;
}

int ipv4_socket_bind(ipv4_socket_ptr socket) {
  return bind(socket->socket_fd, (struct sockaddr *) &socket->address, sizeof(socket->address));
}

int ipv4_socket_listen(ipv4_socket_ptr socket) {
  return listen(socket->socket_fd, IPV4_SOCKET_BACKLOG_SIZE);
}

int ipv4_socket_accept(ipv4_socket_ptr server_socket, ipv4_socket_ptr client_socket) {
  ipv4_socket placeholder = {0};
  if (client_socket == NULL) {
    client_socket = &placeholder;
  }
  socklen_t client_length = sizeof(client_socket->address);
  int socket_fd = accept(server_socket->socket_fd,
                         (struct sockaddr *) &client_socket->address,
                         &client_length);
  client_socket->socket_fd = socket_fd;
  return socket_fd;
}

int ipv4_socket_connect(ipv4_socket_ptr socket) {
  return connect(socket->socket_fd,
                 (const struct sockaddr *) &socket->address,
                 sizeof(socket->address));
}

bool ipv4_socket_create_and_connect(string ip_address, uint16_t port_number, ipv4_socket_ptr socket_out) {
  uint32_t binary_ip = inet_addr(ip_address);
  if (ipv4_socket_create(port_number, (struct in_addr) {binary_ip}, socket_out) < 0) {
      report_error("Couldn't create new socket to connect to client with"
                   "I.P: %s and Port: %" PRIu16,
                   ip_address, port_number);
      return false;
  }
  if (ipv4_socket_connect(socket_out) < 0) {
      report_error("Couldn't connect to client with I.P: %s and Port: %" PRIu16,
                   ip_address, port_number);
      return false;
  }
  return true;
}

ssize_t ipv4_socket_send_request(ipv4_socket_ptr receiver, request request) {
  uint32_t bytes = request.header.bytes;
  request.header = header_hton(request.header);
  if (write(receiver->socket_fd, &request.header, sizeof(request_header)) < 0) {
      return -1;
  }
  if (write(receiver->socket_fd, request.data, bytes) < 0) {
      return -1;
  }
  return 0;
}

request ipv4_socket_get_request(ipv4_socket_ptr sender) {
  request_header header = {0};
  int res;
  if ((res = read(sender->socket_fd, &header, sizeof(request_header))) <= 0) {
    report_warning("Read returned %d in get_request while getting the header", res);
    return (request) {0};
  }
  header = header_ntoh(header);

  int8_t *data = __MALLOC__(header.bytes, byte);

  if ((res = read(sender->socket_fd, data, header.bytes)) <= 0) {
    report_warning("Read returned %d in get_request while getting the data", res);
    __FREE__(data);
    return (request) {0};
  }

  request result = {
          .data = data,
          .header = header
  };
  return result;
}

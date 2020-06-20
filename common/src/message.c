#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "../includes/ipv4_socket.h"
#include "../includes/macros.h"
#include "../includes/message.h"
#include "../includes/report_utils.h"

message message_create(const char *restrict query, int id) {
  message_header header = {
    .bytes = __MESSAGE_BYTES(query),
    .id = (uint8_t) id
  };

  uint8_t *data = __MALLOC__(header.bytes, uint8_t);

  memcpy(data, query, header.bytes);

  return (message) {
    .header = header,
    .data = data
  };
}

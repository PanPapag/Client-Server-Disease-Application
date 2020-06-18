#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "../includes/ipv4_socket.h"
#include "../includes/macros.h"
#include "../includes/message.h"
#include "../includes/report_utils.h"

message create_query_message(const char *restrict query) {
  message_header header = {
    .bytes = __MESSAGE_BYTES(query),
    .id = 'C'
  };

  uint8_t *data = __MALLOC__(header.bytes, uint8_t);

  memcpy(data, query, header.bytes);

  return (message) {
    .header = header,
    .data = data
  };
}

message create_statistics_message(const char *restrict statistics) {
  message_header header = {
    .bytes = __MESSAGE_BYTES(statistics),
    .id = 'W'
  };

  uint8_t *data = __MALLOC__(header.bytes, uint8_t);

  memcpy(data, statistics, header.bytes);

  return (message) {
    .header = header,
    .data = data
  };
}

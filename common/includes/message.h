#ifndef __COMMON_MESSAGE__
  #define __COMMON_MESSAGE__

  #include <stddef.h>
  #include <string.h>
  #include <netinet/in.h>

  #include "attributes.h"
  #include "macros.h"
  #include "types.h"

  #define QUERY 'Q'
  #define NUM_STATISTICS 'N'
  #define STATISTICS 'S'
  #define RESPONSE 'R'

  #define __MESSAGE_BYTES(message) (strlen(message) + 1 ) * sizeof(char)

  typedef struct message_header {
      uint32_t bytes;
      char id;
  } message_header;

  typedef struct message {
      message_header header;
      int8_t *data;
  } message;

  typedef message* message_ptr;

  static __INLINE__
  message_header header_hton(message_header header) {
      return (message_header) {
              .bytes = htonl(header.bytes),
              .id = header.id
      };
  }

  static __INLINE__
  message_header header_ntoh(message_header header) {
      return (message_header) {
              .bytes = ntohl(header.bytes),
              .id = header.id
      };
  }

  static __INLINE__
  void free_message(message_ptr message) {
    __FREE__(message->data);
  }

  message create_query_message(const char *restrict query);

  message create_num_statistics_message(const char *restrict num_statistics);

  message create_statistics_message(const char *restrict statistics);

#endif

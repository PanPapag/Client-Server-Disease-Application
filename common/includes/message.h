#ifndef __COMMON_MESSAGE__
  #define __COMMON_MESSAGE__

  #include <stddef.h>
  #include <string.h>
  #include <netinet/in.h>

  #include "attributes.h"
  #include "macros.h"
  #include "types.h"

  enum message_header_id {
    HOSTNAME_AND_PORT,
    QUERY,
    NUM_STATISTICS,
    STATISTICS,
    RESPONSE
  };

  #define __MESSAGE_BYTES(message) (strlen(message) + 1 ) * sizeof(char)

  typedef struct message_header {
      uint32_t bytes;
      uint8_t id;
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
  void message_destroy(message_ptr message) {
    __FREE__(message->data);
  }

  message message_create(const char *restrict query, int id);

#endif

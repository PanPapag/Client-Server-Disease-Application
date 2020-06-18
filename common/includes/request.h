#ifndef __COMMON_REQUEST__
  #define __COMMON_REQUEST__

  #include <stddef.h>
  #include <netinet/in.h>

  #include "attributes.h"
  #include "macros.h"
  #include "types.h"

  #define __COMMAND_LENGTH(command) __##command##_LENGTH

  typedef struct request_header {
      uint32_t bytes;
      uint32_t command_length;
  } request_header;

  typedef struct request {
      request_header header;
      int8_t *data;
  } request;

  typedef request* request_ptr;

  static __INLINE__
  request_header header_hton(request_header header) {
      return (request_header) {
              .bytes = htonl(header.bytes),
              .command_length = htonl(header.command_length)
      };
  }

  static __INLINE__
  request_header header_ntoh(request_header header) {
      return (request_header) {
              .bytes = ntohl(header.bytes),
              .command_length = ntohl(header.command_length)
      };
  }

  static __INLINE__
  void free_request(request_ptr request) {
    __FREE__(request->data);
  }

#endif

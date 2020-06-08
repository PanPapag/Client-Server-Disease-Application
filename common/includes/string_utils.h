#ifndef __COMMON_STRING_UTILS__
  #define __COMMON_STRING_UTILS__

  #include <stdbool.h>
  #include <stddef.h>
  #include <stdint.h>

  #include "attributes.h"
  #include "types.h"

  __NON_NULL__(1, 2)
  bool str_to_ui64_with_end_ptr(const char *source, uint64_t *destination, char **end_ptr);

  __NON_NULL__(1, 2)
  bool str_to_uint64(const char *source, uint64_t *destination);

#endif


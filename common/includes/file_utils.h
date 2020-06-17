#ifndef __COMMON_FILE_UTILS__
  #define __COMMON_FILE_UTILS__

  #include <stdbool.h>

  #include "attributes.h"
  #include "list.h"

  typedef struct entire_file {
    void *data;
    size_t size;
  } entire_file;

  __NON_NULL__(1)
  list_ptr get_subdirs(const char *restrict dirname);

  __NON_NULL__(1)
  entire_file read_entire_file_into_memory(const char *filename);

  __NON_NULL__(1)
  void get_file_lines(const char *filename, int *num_lines);

  __NON_NULL__(1)
  bool file_exists(const char *restrict path);

  __NON_NULL__(1)
  bool directory_exists(const char *restrict path);

#endif

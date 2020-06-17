#ifndef __COMMON_MACROS__
	#define __COMMON_MACROS__

	#include <stdlib.h>

	#define MAX(x, y) ((x) < (y) ? (y) : (x))

	#define __MALLOC__(size, type) ((type *) malloc((size) * sizeof(type)))
	#define __CALLOC__(size, type) ((type *) calloc(size, sizeof(type)))

  #define __FREE__(ptr) \
  do {                  \
    free(ptr);          \
    ptr = NULL;         \
  } while(0)

#endif

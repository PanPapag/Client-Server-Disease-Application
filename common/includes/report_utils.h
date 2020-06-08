#ifndef __COMMON_REPORT_UTILS__
  #define __COMMON_REPORT_UTILS__

  #include "attributes.h"
  
  __FORMAT__(printf, 1, 2)
  void die(const char *fmt, ...);

  __FORMAT__(printf, 1, 2)
  void report_error(const char *fmt, ...);

  __FORMAT__(printf, 1, 2)
  void report_response(const char *fmt, ...);

#endif
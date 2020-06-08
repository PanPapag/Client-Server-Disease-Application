#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "../includes/string_utils.h"

bool str_to_ui64_with_end_ptr(const char *source, uint64_t *destination, char **end_ptr) {
    errno = 0;
    uint64_t value = strtol(source, end_ptr, 10);
    if ((errno == ERANGE && value == LONG_MAX)
        || (errno != 0 && value == 0U)
        || (end_ptr != NULL && *end_ptr == source)) {
        return false;
    }
    *destination = value;
    return true;
}

bool str_to_uint64(const char *source, uint64_t *destination) {
    return str_to_ui64_with_end_ptr(source, destination, NULL);
}
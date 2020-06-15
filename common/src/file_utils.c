#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../includes/constants.h"
#include "../includes/file_utils.h"
#include "../includes/macros.h"
#include "../includes/report_utils.h"

entire_file read_entire_file_into_memory(const char *filename) {
  entire_file file = {0};
  struct stat info = {0};
  if (stat(filename, &info) == -1) return file;
  size_t length = info.st_size;
  void *data = __MALLOC__(length + 1, int8_t);
  if (data == NULL) return file;
  int fd = open(filename, O_RDONLY);
  if (fd == -1) return file;
  if (read(fd, data, length) == -1) return file;
  file.data = data;
  file.size = length;
  return file;
}

void get_file_lines(const char *filename, int *num_lines) {
  char command[MAX_BUFFER_SIZE];
  sprintf(command, "cat %s | wc -l", filename);
  FILE *fp = popen(command, "r");
  fscanf(fp, "%d", num_lines);
  pclose(fp);
}

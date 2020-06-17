#include <dirent.h>
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
#include "../includes/list.h"
#include "../includes/macros.h"
#include "../includes/report_utils.h"
#include "../includes/types.h"
#include "../includes/utils.h"

list_ptr get_subdirs(const char *restrict dirname) {
  list_ptr result = list_create(string*, ptr_to_string_compare,
                                ptr_to_string_print, ptr_to_string_destroy);

  struct stat path_stat;
  struct dirent *direntp;
  DIR *dir_ptr = opendir(dirname);
  if (dir_ptr == NULL) {
    report_error("Cannot open %s", dirname);
  }
  else {
    while ((direntp = readdir(dir_ptr)) != NULL) {
      // Skip previous and current folder
      if (strcmp(direntp->d_name, ".") && strcmp(direntp->d_name, "..")) {
        char *full_path = concat(2, dirname, direntp->d_name);
        stat(full_path, &path_stat);
        if (S_ISDIR(path_stat.st_mode)){
          list_push_back(&result, &full_path);
        }
      }
    }
    closedir(dir_ptr);
  }
  return result;
}

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

bool file_exists(const char *restrict path) {
  struct stat info = {0};
  return stat(path, &info) == 0;
}

bool directory_exists(const char *restrict path) {
  struct stat info = {0};
  int res = stat(path, &info);
  return res == 0 && ((info.st_mode & S_IFDIR) != 0);
}

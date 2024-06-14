#include <dirent.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc < 4) {
    fprintf(stderr, "Bad arg! (use \"./a.out ./ "
                    "prefix extension\")\n");
    exit(1);
  }

  char *dir = argv[1];
  char *prefix = argv[2];
  char *extension = argv[3];
  int len = strlen(prefix) + strlen(extension) + 15;
  char pattern[len];
  memset(pattern, 0, len);
  regex_t regex;
  snprintf(pattern, len, "^%s_[0-9]{10}\\.%s$", prefix, extension);
  printf("pattern: %s\n", pattern);
  int res = regcomp(&regex, pattern, REG_EXTENDED);

  if (res) {
    fprintf(stderr, "regex is not compile\n");
    regfree(&regex);
    exit(1);
  }

  DIR *dp;
  struct dirent *entry;
  if ((dp = opendir(dir)) == NULL) {
    fprintf(stderr, "cannot open: %s\n", dir);
    exit(1);
  }

  time_t time = 0;
  char *filename = NULL;
  while ((entry = readdir(dp)) != NULL) {
    res = regexec(&regex, entry->d_name, 0, NULL, 0);
    if (!res) {
      char *pos = strchr(entry->d_name, '_') + 1;
      char t[11];
      strncpy(t, pos, 10);

      time_t temp = strtol(t, NULL, 10);
      if (temp > time) {
        time = temp;
        filename = entry->d_name;
      }
    }
  }

  closedir(dp);
  regfree(&regex);
  if (!time || !filename) {
    fprintf(stderr, "no file match\n");
    exit(1);
  }

  FILE *fp = fopen(filename, "r");
  if (!fp) {
    fprintf(stderr, "cannot open: %s\n", filename);
    exit(1);
  }

  char c = fgetc(fp);
  while (c != EOF) {
    printf("%c", c);
    c = fgetc(fp);
  }
}
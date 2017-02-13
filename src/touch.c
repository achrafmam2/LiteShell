#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

/* usage: prints how to use 'mkdir' command. */
void usage(void);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    usage();
    exit(0);
  }
  
  for (int i = 1; i < argc; i++) {
    mode_t mode = S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH; // mode = rwx-rx-r.
    int fd = open(argv[i], O_RDONLY | O_CREAT, mode);
    if (fd < 0) {
      fprintf(stderr, "touch: failed to create file %s.\n", argv[i]);
      continue;
    }
    close(fd);
  }
  
  return 0;
}

void usage(void) {
  fprintf(stderr, "usage: touch file ...\n");
}
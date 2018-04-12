#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

/*
  usage: prints how to use 'mkdir' command.
*/
void usage(void);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    usage();
    exit(EXIT_SUCCESS);
  }
  
  for (int i = 1; i < argc; i++) {
    int status = mkdir(argv[i], S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH);
    if (status) {
      fprintf(stderr, "mkdir: failed to create directory %s\n", argv[i]);
    }
  }
  
  return 0;
}

void usage(void) {
  fprintf(stderr, "usage: mkdir directory ...\n");
}



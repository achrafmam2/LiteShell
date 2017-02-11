#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>

/*
  usage: prints how to use 'ls' command.
*/
void usage(void);

/*
  showdir: show directory content.
  @param path: directory path to process.
  @param hflag: should print hidden files?
*/
void showdir(const char *path, int hflag);

int main(int argc, char *argv[]) {
  int hflag = 0;
  
  opterr = 0;
  
  int c;
  while ((c = getopt(argc, argv, "h")) != -1) {
    switch(c) {
      case 'h':
        hflag = 1;
        break;
      case '?':
        if (isprint(optopt)) {
          fprintf(stderr, "Unknown option '-%c'\n", optopt);
        } else {
          fprintf(stderr, "Unknown option ‘\\x%x’.\n", optopt);
        }
        usage();
        exit(EXIT_SUCCESS);
      default:
        abort();
    }
  }
  
  int is_path_given = 0;
  for (int i = optind; i < argc; i++) {
    is_path_given = 1;
    
    if (argc - optind > 1) {
      // More than one directory.
      printf("%s:\n", argv[i]);
    }
    showdir(argv[i], hflag);
    if (i + 1 < argc) {
      puts("");
    }
  }
  
  if (!is_path_given) {
    showdir("./", hflag);
  }
  
  exit(EXIT_SUCCESS);
}

void usage(void) {
  fprintf(stderr, "ls: [-a] [file ...]\n");
}

void showdir(const char *path, int hflag) {
  DIR *dp = opendir(path);
  struct dirent *ep;
  
  if (dp != NULL) {
    while ((ep = readdir(dp))) {
      int print = 0;
      if (ep->d_name[0] == '.') {
        if (hflag) print = 1;
      } else {
        print = 1;
      }
      if (print) puts(ep->d_name);
    }
    closedir(dp);
  } else {
    fprintf(stderr, "ls: %s: no such directory.\n", path);
  }
}
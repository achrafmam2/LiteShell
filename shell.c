#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* 
  getcmd: get input from stdin.
  @param buff: buffer for input from stdin.
  @param limit: maximum characters in the buffer.
  @return: 1 in case of success, 0 otherwise.
*/
int getcmd(char *buff, int limit);

/*
  fork1: create a new process.
  @return: process id, or -1 in case of failure.
*/
int fork1(void);

/* struct cmd: encapsulated command information */
struct command {
  char *path;
  char **argv;
  int nargs;
};

/*
  parsecmd: parses a string and creates a cmd out of it 
  @param buff: buffer that contains the command and it arguments.
  @return: a struct command in case of success, NULL otherwise.
*/
struct command *parsecmd(char *buff);

/*
  execcmd: execute a cmd, and exit process.
  @param cmd: command to be executed.
*/
void execcmd(struct command*);

int main(int argc, char *argv[]) {
  const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE+1];
  
  while (getcmd(buffer, BUFFER_SIZE)) {
     if (fork1() == 0) {
       parsecmd(buffer);
     }
  }
  
  puts("Session closed.");
  
  exit(EXIT_SUCCESS);
}

int getcmd(char *buff, int limit) {
  if (isatty(fileno(stdin))) {
    // if the input comes from a terminal, print name of the user who is logged in the current
    // session.
    printf("%s$ ", getlogin());
  }
  
  if (!fgets(buff, limit, stdin)) {
    return 0;
  }
  
  // Trim end of line if any.
  if (buff[strlen(buff)-1] == '\n') {
    buff[strlen(buff)-1] = '\0';
  }
  
#ifdef DEBUG
  fprintf(stderr, "buff: %s\n", buff);
#endif
  
  return 1;
}

int fork1(void) {
  pid_t pid = fork();
  
  if (pid == -1) {
    fprintf(stderr, "fork1 error.\n");
    exit(EXIT_FAILURE);
  }
  
  return pid;
}

struct command * parsecmd(char *buff) {
  char *delims = " ";
  char *s = strtok(buff, delims);
  
  struct command *cmd = (struct command *)malloc(sizeof(struct command));
  if (!cmd) {
    fprintf(stderr, "parsecmd: couldn't allocate memory for cmd.\n");
    return NULL;
  }
  
  int argc = 0, argv_cap = 0;
  while (s) {
    if (argc == 0) {
      // Command path.
      cmd->path = (char *)malloc(sizeof(char) * (1 + strlen(s)));
      strcpy(cmd->path, s);
#ifdef DEBUG
      fprintf(stderr, "cmd: %s\n", cmd->path);
#endif
    } else {
      // Command arguments;
      cmd->nargs++;
      if (cmd->nargs > argv_cap) {
        // Not enough capacity, double it.
        argv_cap = MAX(argv_cap + 1, argv_cap * 2);
        if (!(cmd->argv = realloc(cmd->argv, argv_cap * sizeof(char *)))) {
          fprintf(stderr, "parsecmd: not enough memory for arguments.\n");
          return NULL;
        }
      }
      // Copy it.
      cmd->argv[argc-1] = (char *)malloc(sizeof(char) * (1 + strlen(s))); 
      strcpy(cmd->argv[argc-1], s);
#ifdef DEBUG
      fprintf(stderr, "arg%i: %s\n", argc, cmd->argv[argc-1]);
#endif
    }
    
    s = strtok(NULL, delims);
    argc++;
  }
  
  return cmd;
}
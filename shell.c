#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
struct cmd {
  char *path;
  char *argv;
  int nargs;
};

/*
  parsecmd: parses a string and creates a cmd out of it 
  @param buff: buffer that contains the command and it arguments.
  @return: a struct cmd in case of success, NULL otherwise.
*/
struct cmd *parsecmd(char *buff);

/*
  execcmd: execute a cmd, and exit process.
  @param cmd: command to be executed.
*/
void execcmd(struct cmd*);

int main(int argc, char *argv[]) {
  const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE+1];
  
  while (getcmd(buffer, BUFFER_SIZE)) {
     if (fork1() == 0) {
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
  
  // Trim end of line is any.
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
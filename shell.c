#include <assert.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
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
struct command {
  /* The command path is argv[0]. */
  char **argv; /* arguments of the command, NULL terminated */   
};

/*
  parsecmd: parses a string and creates a cmd out of it, terminates in case of error.
  @param buff: buffer that contains the command and it arguments.
  @return: a struct command in case of success.
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
       execcmd(parsecmd(buffer)); // Never returns.
     }
     wait(NULL);
  }
  
  puts("Session closed.");
  
  exit(EXIT_SUCCESS);
}

/*
  getrcwd: get relative path of current working directory.
  @return: pointer to relative current working directory.
  @note: caller is responsible of freeing memory.
*/

char *getrcwd(void) {
  static size_t MAXPATH_LEN = 1024;
  
  char *cwd = (char *)malloc(sizeof(char) * (MAXPATH_LEN + 1));
  cwd = getcwd(cwd, MAXPATH_LEN);
  
  // if current working directory is the home directory change to '~'.
  struct passwd *pw = getpwuid(getuid());
  const char *homedir = pw->pw_dir;
  if (!strcmp(cwd, homedir)) {
    strcpy(cwd, "~");
  } else {
    int i;
    
    // locate last '/'.
    for (i = strlen(cwd)-1; i >= 0; i--) {
      if (cwd[i] == '/') {
        break;
      }
    }   
    assert(i >= 0);
    
    // Copy relative directory path.
    strcpy(cwd, &cwd[i+1]);
    
    if (strlen(cwd) == 0) {
      // We are in root now.
      strcpy(cwd, "/");
    }
  }
    
  return cwd;
}

int getcmd(char *buff, int limit) {
  if (isatty(fileno(stdin))) {
    // if the input comes from a terminal, print name of the user who is logged in the current
    // session.
    char *working_dir = getrcwd();
    printf("%s: %s$ ", working_dir, getlogin());
    free(working_dir);
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
    exit(EXIT_FAILURE);
  }
  
  // Allocate memory for argv[].
  cmd->argv = (char **)malloc(sizeof(char *));
  if (cmd->argv == NULL) {
    fprintf(stderr, "parsecmd: Not enough memory for argv[].\n");
    exit(EXIT_FAILURE);
  }
  
  // Parse buff.
  int argc = 0, argv_cap = 1;
  while (s) {
    if (argv_cap <= argc + 1) {
      // Double capacity.
      argv_cap *= 2;
      
      cmd->argv = (char **)realloc(cmd->argv, sizeof(char *) * (argv_cap + 1)); // +1 for NULL.
      if (!cmd->argv) {
        fprintf(stderr, "parsecmd: Not enough memory for argv[].\n");
        exit(EXIT_FAILURE);
      }
    }
    
    // Copy it.
    cmd->argv[argc] = (char *)malloc(sizeof(char) * (1 + strlen(s))); 
    strcpy(cmd->argv[argc], s);
    
    s = strtok(NULL, delims);
    argc++;
  }
  
  // NULL terminate argv[].
  cmd->argv[argc] = NULL;
  
#ifdef DEBUG
  for (int i = 0; cmd->argv[i]; i++) {
    printf("$arg %i: %s\n", i, cmd->argv[i]); 
  }
#endif
    
  return cmd;
}

bool is_path(const char *s) {
  for (int i = 0; i < strlen(s); i++) {
    if (s[i] == '/') {
      return true;
    }
  }
  return false;
}

char *get_cmd_path(char *s) {
  if (is_path(s)) {
    return s;
  }
  
  // Look for command 's' in $LITE_SHELL_PATH.
  char *paths = getenv("LITE_SHELL_PATH");
  if (paths) {
    const char *delim = ":"; // Paths are separated with ':'.

    char *ps = strtok(s, delim);
    while (ps) {
      char *path = (char *)malloc(sizeof(char) * (strlen(ps) + strlen(s) + 1));
      strcpy(path, ps);
      strcat(path, s);
      
      // Check if file exists.
      if (!access(path, F_OK)) {
        return path;
      }
      
      ps = strtok(NULL, delim);
    }
  }
  
  return NULL;
}

void execcmd(struct command *cmd) {
  const char *cmd_path = get_cmd_path(cmd->argv[0]);
  if (!cmd_path) {
    fprintf(stderr, "Command not found.\n");
    _exit(127);
  }
    
  execv(cmd_path, cmd->argv);
  
#ifdef DEBUG
  fprintf(stderr, "execcmd: execv failed.\n");
#endif
  _exit(127);
}
#include "exec.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

_Bool exec_special(char **args);

void exec_single_program_args(char ** args, int stdin, int stdout, int stderr);

void pipe_args(char** args);

void exec_args(char ** args) {
    int stdout_fd = STDOUT_FILENO;
    int stdin_fd = STDIN_FILENO;
    int stderr_fd = STDERR_FILENO;

    char **program_args = args;

    if(!args) return;
    for(char **pos = args; *pos != NULL; pos++) {
        if (strcmp(*pos, ">") == 0 || strcmp(*pos, "1>") == 0) {
            if(pos[1] == NULL) {
                printf("Error: must specify file to redirect to.\n");
                return;
            }
            int fd = open(pos[1], O_CREAT | O_TRUNC | O_RDWR);
            if (fd < 0) {
                printf("Error: %s\n", strerror(errno));
                return;
            }
            stdout_fd = fd;
            *pos = NULL;
            break;
        }
        
        else if (strcmp(*pos, "2>") == 0) {
            if(pos[1] == NULL) {
                printf("Error: must specify file to redirect to.\n");
                return;
            }
            int fd = open(pos[1], O_CREAT | O_TRUNC | O_RDWR);
            if (fd < 0) {
                printf("Error: %s\n", strerror(errno));
                return;
            }
            stderr_fd = fd;
            *pos = NULL;
            break;
        }
        else if (strcmp(*pos, "&>") == 0) {
            printf("test: found >>\n");
            if(pos[1] == NULL) {
                printf("Error: must specify file to redirect to.\n");
                return;
            }
            int fd = open(pos[1], O_CREAT | O_TRUNC | O_RDWR);
            if (fd < 0) {
                printf("Error: %s\n", strerror(errno));
                return;
            }
            stdout_fd = fd;
            stderr_fd = fd;
            *pos = NULL;
            break;
        }

        else if (strcmp(*pos, ">>") == 0 || strcmp(*pos, "1>>") == 0) {
            printf("test: found >>\n");
            if(pos[1] == NULL) {
                printf("Error: must specify file to redirect to.\n");
                return;
            }
            int fd = open(pos[1], O_CREAT | O_APPEND | O_RDWR);
            if (fd < 0) {
                printf("Error: %s\n", strerror(errno));
                return;
            }
            stdout_fd = fd;
            *pos = NULL;
            break;
        }
        else if (strcmp(*pos, "2>>") == 0) {
            printf("test: found >>\n");
            if(pos[1] == NULL) {
                printf("Error: must specify file to redirect to.\n");
                return;
            }
            int fd = open(pos[1], O_CREAT | O_APPEND | O_RDWR);
            if (fd < 0) {
                printf("Error: %s\n", strerror(errno));
                return;
            }
            stderr_fd = fd;
            *pos = NULL;
            break;
        }
        else if (strcmp(*pos, "&>>") == 0) {
            printf("test: found >>\n");
            if(pos[1] == NULL) {
                printf("Error: must specify file to redirect to.\n");
                return;
            }
            int fd = open(pos[1], O_CREAT | O_APPEND | O_RDWR);
            if (fd < 0) {
                printf("Error: %s\n", strerror(errno));
                return;
            }
            stdout_fd = fd;
            stderr_fd = fd;
            *pos = NULL;
            break;
        }

        else if (strcmp(*pos, ";") == 0) {
            *pos = NULL;
            exec_single_program_args(program_args, stdin_fd, stdout_fd, stderr_fd);
            exec_single_program_args(pos + 1, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO);
            return;
        }
    }
    exec_single_program_args(args, stdin_fd, stdout_fd, stderr_fd);

    if (stdin_fd != STDIN_FILENO) {
        close(stdin_fd);
    }
    if (stdout_fd != STDOUT_FILENO) {
        close(stdout_fd);
    }
    if (stderr_fd != STDERR_FILENO) {
        close(stderr_fd);
    }
}

void pipe_args (char** args){
    int stdout_fd = STDOUT_FILENO;
    int stdin_fd = STDIN_FILENO;
    int stderr_fd = STDERR_FILENO;
    
    if(!args) return;
    for(char **pos = args; *pos != NULL; pos++) {
        if (strcmp(*pos, "| ") == 0) {
            if (pos[-1] == NULL){
                printf("\nError: Must Specify Which File You are Piping from.");
            }
            int fd1 = open(pos[-1], O_CREAT | O_TRUNC | O_RDWR);
            if (fd1 < 0) {
                printf("Error: %s\n", strerror(errno));
                return;
            }
            if (pos[1] == NULL){
                printf("\nError: Must Specify Which File You are Piping to.");
            }
            int fd2 = open(pos[1], O_CREAT | O_TRUNC | O_RDWR);
            if (fd2 < 0) {
                printf("Error: %s\n", strerror(errno));
                return;
            }
            stdin_fd = fd1;
            stdout_fd = fd2;
            stderr_fd = fd2;
            *pos = NULL;
            break;
        }
    }

    exec_single_program_args(args, stdin_fd, stdout_fd, stderr_fd);

    if (stdin_fd != STDIN_FILENO) {
          close(stdin_fd);
    }
    if (stdout_fd != STDOUT_FILENO) {
          close(stdout_fd);
    }
    if (stderr_fd != STDERR_FILENO) {
           close(stderr_fd);
           }
}


//args: program name and its arguments
//int stdin, stdout, stderr: file descriptors to use for child
void exec_single_program_args(char ** args, int stdin, int stdout, int stderr) {
    pid_t parent;
    int status;

    if(exec_special(args)) return;

    child_process = fork();
    if (child_process == 0) {
        dup2(stdin, STDIN_FILENO);
        dup2(stdout, STDOUT_FILENO);
        dup2(stderr, STDERR_FILENO);
        if (execvp(args[0], args) < 0)
            printf("Error: Execution failed.\n");
        exit(0);
    }

    else if (child_process == -1) printf("Error: child could not be forked.\n");
    else {
        parent = waitpid(child_process, &status, 0);
        //sleep(1);
        printf("  test: waitpid returned  ");
    }
    
    //indicate to terminate_exec no process is running
    child_process = -1;
}

void terminate_exec() {
    if(child_process > 0) {
        kill(child_process, SIGINT);
    }
}

//returns true if special command executed
_Bool exec_special(char **args) {
    if(args[0] == NULL || strcmp(args[0], "") == 0) {
        return true;
    }
    else if(strcmp(args[0], "exit") == 0) {
        //TODO: impl numeric return
        if (args[1] == NULL) exit(0);
        else {
            fprintf(stderr, "Error: exit expects 0 arguments.\n");
            return true;
        }
    }
    else if(strcmp(args[0], "cd") == 0) {
        if(args[1] != NULL && args[2] != NULL) {
            fprintf(stderr, "Error: cd expects 0 or 1 arguments.\n");
        }
        else if(args[1] == NULL) {
            char *home = getenv("HOME");
            int err;
            if(home) {
                chdir(home);
            }
            else chdir("/");
            if(err < 0) {
                fprintf(stderr, "Error: %s.\n", strerror(errno));
            }
        }
        else {
            if (chdir(args[1]) < 0) {
                fprintf(stderr, "Error: %s.\n", strerror(errno));
            }
        }
        return true;
    }
    else return false;
}




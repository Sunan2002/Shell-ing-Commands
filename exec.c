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

//args: args[0] is program name, arg[last] = NULL
//executes built-in commands like help and exit
//returns true if special program executed, false otherwise
_Bool exec_special(char **args);

//args: standard args format
//stdin, stdout, stderr: file descriptors used for program's std out, in, and err respectively
//executes given program with args in a forked process
void exec_single_program_args(char ** args, int stdin, int stdout, int stderr);

//first_program, second_program: args for executing each program in standard format
//executes first_program[0], then feeds its output to execution of second_program[0]
void pipe_args(char **first_program, char **second_program);

//documented in exec.h
void exec_args(char ** args) {
    int stdout_fd = STDOUT_FILENO;
    int stdin_fd = STDIN_FILENO;
    int stderr_fd = STDERR_FILENO;

    char **program_args = args;

    if(!args) return;

    //mode for new files
    mode_t mode = 0666;

    for(char **pos = args; *pos != NULL; pos++) {
        if (strcmp(*pos, ">") == 0 || strcmp(*pos, "1>") == 0) {
            if(pos[1] == NULL) {
                printf("Error: must specify file to redirect to.\n");
                return;
            }
            int fd = open(pos[1], O_CREAT | O_TRUNC | O_RDWR, mode);
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
            int fd = open(pos[1], O_CREAT | O_TRUNC | O_RDWR, mode);
            if (fd < 0) {
                printf("Error: %s\n", strerror(errno));
                return;
            }
            stderr_fd = fd;
            *pos = NULL;
            break;
        }
        else if (strcmp(*pos, "&>") == 0) {
            if(pos[1] == NULL) {
                printf("Error: must specify file to redirect to.\n");
                return;
            }
            int fd = open(pos[1], O_CREAT | O_TRUNC | O_RDWR, mode);
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
            if(pos[1] == NULL) {
                printf("Error: must specify file to redirect to.\n");
                return;
            }
            int fd = open(pos[1], O_CREAT | O_APPEND | O_RDWR, mode);
            if (fd < 0) {
                printf("Error: %s\n", strerror(errno));
                return;
            }
            stdout_fd = fd;
            *pos = NULL;
            break;
        }
        else if (strcmp(*pos, "2>>") == 0) {
            if(pos[1] == NULL) {
                printf("Error: must specify file to redirect to.\n");
                return;
            }
            int fd = open(pos[1], O_CREAT | O_APPEND | O_RDWR, mode);
            if (fd < 0) {
                printf("Error: %s\n", strerror(errno));
                return;
            }
            stderr_fd = fd;
            *pos = NULL;
            break;
        }
        else if (strcmp(*pos, "&>>") == 0) {
            if(pos[1] == NULL) {
                printf("Error: must specify file to redirect to.\n");
                return;
            }
            int fd = open(pos[1], O_CREAT | O_APPEND | O_RDWR, mode);
            if (fd < 0) {
                printf("Error: %s\n", strerror(errno));
                return;
            }
            stdout_fd = fd;
            stderr_fd = fd;
            *pos = NULL;
            break;
        }

        else if (strcmp(*pos, "<") == 0) {
            if(pos[1] == NULL) {
                printf("Error: must specify file to redirect from.\n");
                return;
            }
            int fd = open(pos[1], O_RDONLY);
            if (fd < 0) {
                printf("Error: %s\n", strerror(errno));
                return;
            }
            stdin_fd = fd;
            *pos = NULL;
        }

        else if(strcmp(*pos, "|") == 0) {
            if(pos[1] == NULL) {
                printf("Error: must specify program to pipe to.\n");
                return;
            }

            *pos = NULL;

            pipe_args(args, pos + 1);
            return;
        }

        else if (strcmp(*pos, ";") == 0) {
            *pos = NULL;
            exec_single_program_args(program_args, stdin_fd, stdout_fd, stderr_fd);
            exec_args(pos + 1);
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

void pipe_args (char **first_args, char **second_args) {
    //inserted a pipe
    int fds[2];
    if (pipe(fds) == -1) {
        fprintf(stderr, "Error: failed to initialize pipes.\n");
        return;
    }

    int READ = fds[0];
    int WRITE = fds[1];

    exec_single_program_args(first_args, STDIN_FILENO, WRITE, STDERR_FILENO);
    close(WRITE);

    exec_single_program_args(second_args, READ, STDOUT_FILENO, STDERR_FILENO);
    close(READ);
    return;
}
    

//args: program name and its arguments
//int stdin, stdout, stderr: file descriptors to use for child
void exec_single_program_args(char **args, int stdin, int stdout, int stderr) {
    pid_t parent;
    int status;

    if(exec_special(args)) return;

    child_process = fork();
    if (child_process == 0) {
        dup2(stdin, STDIN_FILENO);
        dup2(stdout, STDOUT_FILENO);
        dup2(stderr, STDERR_FILENO);
        if (execvp(args[0], args) < 0)
            printf("Error: %s.\n", strerror(errno));
        exit(0);
    }

    else if (child_process == -1) {
        printf("Error: child could not be forked.\n");
        return;
    }
    
    else {
        while(true) {
            parent = waitpid(child_process, &status, 0);

            //handles ^C interrupting waitpid
            if(parent < 0 && errno == EINTR) continue;
            else break;
        }
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
    else if(strcmp(args[0], "help") == 0) {
        if (args[1] == NULL) {
            printf("This is the chocomilk shell:\n");
            printf("List of commands available in the shell:\n");
            printf(" cd\n help\n exit");
            printf("\n\nRedirection Commands:");
            printf("\n > \n 2> \n &> \n >> \n 2>> \n &>> \n <");
            printf("\n\nPiping Commands: \n | \n\n");
            printf("For help with a specific command, run help <command>.\n");
        }
        else if(strcmp(args[1], "cd") == 0) {
            printf("cd: changes the working directory to the user's home directory." 
            "\n cd <name of directory>: changes the working directory to the given directory.\n");
        }
        else if(strcmp(args[1], "exit") == 0) {
            printf("exit: exits the shell.\n");
        }
        else if(strcmp(args[1], "help") == 0) {
            printf("help: lists general chocomilk help.\n help <command>: gives help for built-in commands.\n");
        }
        else printf("Error: %s is not a built-in chocomilk command.\n", args[1]);
        return true;
    }
    else return false;
}




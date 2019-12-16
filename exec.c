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

        else if(strcmp(*pos, "|") == 0) {
            printf("test: found |\n");
            if(pos[1] == NULL) {
                printf("Error: must specify file to pipe to.\n");
                return;
            }
            pipe_args(args);
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

   int i = 0;
   while(args[i]) {

       //checking for "|"
       if (strcmp(args[i], "|") == 0 && args[i + 1] != NULL) {
           args[i] = NULL;

           //inserted a pipe
            int fds[2];
            if (pipe(fds) == -1) {
                fprintf(stderr, "Error: failed to initialize pipes.\n");
                return;
            }

            int READ = fds[0];
            int WRITE = fds[1];

            pid_t f;
            f = fork();
            int waiting_for_parent = getpid();

            //child process
            // if(!f) {

            //     //stdout replaced with the WRITE end of pipe
            //     dup2(WRITE, STDOUT_FILENO);

            //     //first program executed into WRITE end
            //     if (execvp(args[0], args) == -1) {
            //         fprintf(stderr, "Error: execution of %s failed: %s.\n", args[0], strerror(errno));
            //         return;
            //     }
            //     exit(0);
            // }
            // //parent process
            // else {
            //     int status;
            //     waitpid(f, &status, 0);
            // }

            exec_single_program_args(args, STDIN_FILENO, WRITE, STDERR_FILENO);

            char **second_program = args + i + 1;

            // pid_t f2;
            // f2 = fork(); //second fork
            // //second child
            // if (!f2) {
            //     waiting_for_parent = getpid();

            //     //stdin replaced with the READ end of pipe
            //     dup2(READ, STDIN_FILENO);

            //     //second program executed into READ end
            //     if (execvp(second_program[0], second_program) == -1){
            //         fprintf(stderr, "Execution Failed");
            //         return;
            //     }
            //     exit(0);
            // }
            // // parent process
            // else { 
            //     int status;
            //     waitpid(f2, &status, 0);
            //     return;
            //     break;
            // }

            exec_single_program_args(second_program, READ, STDOUT_FILENO, STDERR_FILENO);
            close(WRITE);
            close(READ);
        }
        i++;
    }
    return;
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
    else if(strcmp(args[0], "help") == 0) {
        pls_help();
        des_help(args);
    }
    else return false;
}

int pls_help() {
    printf("This is the chocomilk shell:\n");
    printf("List of Commands Available in the Shell:\n");
    printf("\n\n cd   ls   exit");
    printf("\n\nRedirection Commands:");
    printf("\n > \n 2> \n &> \n >> \n 2>> \n &>>");
    printf("\n\nPiping Commands: \n | \n");
    printf("\nPlease Print: help <command name> .");
    return 0;
}

int des_help(char** args){
    if (strcmp(args[0], "help") != 0) {
        fprintf(stderr, "\nPlease Print: help <command name> .");
    }
    if (args[1] == NULL) {
        fprintf(stderr, "\nhelp expects two arguments: help <command name> .");
    }
    else if (strcmp(args[1], ">")) {
        printf(">: Redirects stdout to a file");
    }
    return 0;
}




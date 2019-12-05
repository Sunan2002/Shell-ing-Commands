#include "exec.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

_Bool exec_special(char **args);

void exec_args(char ** args) {
    pid_t child, parent;
    int status;

    if(exec_special(args)) return;

    child = fork();
    if (child == 0) {
        if (execvp(args[0], args) < 0)
            printf("\nError: Execution failed.\n");
    }

    else if (child == -1) printf("\nError: Child could not be forked.\n");
    else parent = waitpid(child, &status, 0);
    //return 1;
}

//returns true if special command executed
_Bool exec_special(char **args) {
    if(strcmp(args[0], "exit") == 0) {
        //TODO: impl numeric return
        if (args[1] == NULL) exit(0);
        else {
            printf("Error: exit expects 0 arguments.\n");
            return true;
        }
    }
    else if(strcmp(args[0], "cd") == 0) {
        if(args[1] != NULL && args[2] != NULL) {
            printf("Error: cd expects 0 or 1 arguments.\n");
        }
        else if(args[1] == NULL) {
            char *home = getenv("HOME");
            int err;
            if(home) {
                chdir(home);
            }
            else chdir("/");
            if(err < 0) {
                printf("Error: %s.\n", strerror(errno));
            }
        }
        else {
            if (chdir(args[1]) < 0) {
                printf("Error: %s.\n", strerror(errno));
            }
        }
        return true;
    }
    else return false;
}




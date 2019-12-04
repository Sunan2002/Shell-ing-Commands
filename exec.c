#include "exec.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void execArgs(char ** args){
    pid_t child, parent;
    int status
    child = fork();
    if (pid == 0){
        if (child == -1)
            printf("\nError: Child Could Not be Forked");
        else if (execvp(args[0], args) < 0)
            printf("\nError: Execution Failed");
        else
            parent = waitpid(pid, &status, options);
    }
    return 1;
}




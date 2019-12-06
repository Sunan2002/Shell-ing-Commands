#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "parser.h"
#include "exec.h"

int main(int argc,  **argv) {
    printf("Welcome to the chocomilk shell! © Darius Jankauskas, Sunan Tajwar 2019\n");
    printf("For help, type help <command_name>.\n");

    //TODO: permit arbitrary size?
    char input[10000];

    while(true) {
        //TODO: print current working directory and user
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        printf("%s", cwd);
        //TODO: check for errors
        fgets(input, sizeof input, stdin);
        strip_newline(input);
        
        char **args = split_input(input);
        if(!args) {
            printf("Parse args returned null");
            continue;
        }

        exec_args(args);

        free(args);
    }

    return 0;
}


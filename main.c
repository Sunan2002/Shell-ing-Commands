#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "parser.h"

int main(int argc, int **argv) {
    printf("Welcome to the chocomilk shell! © Darius Jankauskas, Sunan Tajwar 2019\n");
    printf("For help, type help <command_name>.\n");

    //TODO: permit arbitrary size?
    char input[10000];

    while(true) {
        //TODO: print current working directory and user

        //TODO: check for errors
        fgets(input, sizeof input, stdin);
        strip_newline(input);
        
        char **args = split_input(input);
        if(!args) {
            printf("Parse args returned null");
            continue;
        }

        printf("Testing split_args: \n");
        for(size_t i = 0; args[i] != NULL; i++) {
            printf("%d: %s\n", i, args[i]);
        }

        //TODO: execute command

        free(args);
    }

    return 0;
}
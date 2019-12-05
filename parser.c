#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char** split_input(char *input) {
    size_t cap_args = 8;
    size_t num_args = 0;
    char **args = calloc(cap_args, sizeof(char*));

    char *token = strtok(input, " ");

    while(token  != NULL) {

        //realloc args buffer if too many args
        if(cap_args - 1 == num_args) {
            cap_args *= 2;
            args = realloc(args, cap_args * sizeof(char*));
        }

        printf("cap_args: %d, num_args: %d, token: %s\n", cap_args, num_args, token);

        args[num_args++] = token;

        token = strtok(NULL, " ");
    }
    
    args[num_args] = NULL;
    return args;
}

void strip_newline(char *input) {
    if (!input) return;

    for(size_t i = 0; input[i] != '\0'; i++) {
        if(input[i] == '\n') {
            input[i] = '\0';
            return;
        }
    }
}
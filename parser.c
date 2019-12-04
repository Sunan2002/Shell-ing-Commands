#include "parser.h"

#include <stdlib.h>
#include <stdio.h>

char** split_input(char *input) {
    //TODO: implement
    return NULL;
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
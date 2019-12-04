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
        //TODO: print current working directory

        //TODO: check for errors
        fgets(input, sizeof input, stdin);
        strip_newline(input);
        
        char **args = split_input(input);
        if(!args) continue;

        //TODO: execute command
    }

    return 0;
}
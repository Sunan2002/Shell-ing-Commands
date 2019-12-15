#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

#include "parser.h"
#include "exec.h"

static void sigint_handler(int sig);

int main(int argc, char **argv) {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    
    printf("Welcome to the chocomilk shell! © Darius Jankauskas, Sunan Tajwar 2019\n");
    printf("For help, type help <command_name>.\n");

   //VSCODE does not understand this for some reason
   //does not affect build
    #ifndef __INTELLISENSE__
    struct sigaction sig_handler;
    sig_handler.sa_handler = sigint_handler;
    sigemptyset(&sig_handler.sa_mask);
    sig_handler.sa_flags = 0;
    sigaction(SIGINT, &sig_handler, NULL);
    #endif

    size_t input_size = 1000;
    char *input = malloc(input_size);

    while(true) {
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        printf("MS %s> ", cwd);
        
        //TODO: check for errors
        if ( getline(&input, &input_size, stdin) < 0) {
            break;
        }
        strip_newline(input);
        
        char **args = split_input(input);
        if(!args) {
            printf("Parse args returned null");
            continue;
        }

        exec_args(args);

        free(args);
    }

    free(input);
    return 0;
}

static void sigint_handler(int sig) {
    terminate_exec();
}


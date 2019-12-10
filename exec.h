#ifndef __EXEC_INCLUDES
#define __EXEC_INCLUDES

#include <sys/types.h>

//current child process used to run command
//-1 if error with child process or no child running
pid_t child_process;

//takes in array of args, where args[0] is the name of the program,
//the next args are passed to the program, and args[n_last] == NULL
void exec_args(char ** args);

//if a child process is executing a program, it is sent SIGTERM
//otherwise no action taken
void terminate_exec();

#endif
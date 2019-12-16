# chocomilk
### by Darius Jankauskus and Sunan Tajwar

## Features:
        - Forks and executes commands
        - Parses multiple commands on one line
        - Redirects using >, 2>, &>, >>, 2>>, &>>, <
        - Pipes commands using |
        - Terminate running programs using keyboard interrupt ^C
        - User help within help command

## Attempted:
        - Attempted expand piping to pipe between more than two programs

## Bugs: 
        - Multiple pipes will not execute properly

## Files and Function Headers:

### parser.c
        - Handles all line parsing functions

        ========= char** split_input(char *input) ==========
        Inputs: char* input
        Retuns: char** with all arguments in the command line split by a " "
        Allocates memory for a maximum of 8 command line arguments
        Splits and adds each argument into char** args split by a " "
        Adds NULL to end or char**

        ========== void strip_newline(char *input) =========
        Input: char *input
        Returns: Replaces all "\n" with \0
        Parses through the command line arguments
        If an argument contain "\n" it gets replaced with "\0", thus "stripping the new line"


### exec.c
        Handles the execution of the arguments in the command line

        ========_Bool exec_special(char **args)==========
        Input: char **args
        Returns: True if cd, exit, or help is called, False otherwise
        Parses through the arguments for the built in commands
        Returns error message if there are not the appropriate number of arguments
        Carries to execute commands found and returns true, false otherwise

         ===============================================
         void exec_single_program_args(char ** args, int stdin, int stdout, int stderr)
         ===============================================
         Input: char ** args, int stdin, int stdout, int stderr
         Returns: Forks and executes child process, prints error message if execution fails
         Calls exec_special to check for and run cd, exit, or help if found
         Forks the child process and replaces stdin_fd, stdout_fd, and stderr_fd with STDIN_FILENO, STDOUT_FILENO, and STDERR_FILENO respectively using dup2
         Runs parent process
         Terminates the child process

         ============ void exec_args(char ** args) ==========
         Input: char **args
         Returns: Executes file redirection or pipe if found
         Parses through command line arguments to check for >, 2>, &>, >>, 2>>, &>>, or |
         If any of those arguments are found, the files are redirected or piped accordingly if executed properly, else an error message is returned
         Runs exec_single_program_args
         Closes stdin_fd, stdout_fd, stderr_fd if not equal to STDIN_FILENO, STDOUT_FILENO, and STDERR_FILENO resepctively

         ============= void pipe_args(char ** args) =========
         Input: char **args
         Returns: Pipes the programs in the command line, otherwise returns an error if execution or pipe fails
         Checks for the | argument
         Inserts a pipe with a READ and WRITE end
         Forks and executes the WRITE end using exec_single_program_args
         Forks and executes the READ end using exec_single_program_args
         Closes the WRITE and READ ends

         ============= void terminate_exec() ===========
         If a child process is executing a program, it is sent SIGINT
         Otherwise no action is taken


### main.c
        Handles the reading, parsing, and execution of the inputs in the shell command line

        ======== static void sigint_handler(int sig) =======
        Input: integer value of signal, assumed always SIGNIT
        Returns: Terminates forked process on SIGINT from user, if present, using terminate_exec()

        ========= int main(int argc, char **argv) ===========
        Input: void
        Returns: Executes the command line arguments
        Welcomes user to shell with a printed message
        Prints the current working directory
        Gets the input using fgets and strips newline with strip_newline
        Executes the arguments using exec_args
        Frees the args

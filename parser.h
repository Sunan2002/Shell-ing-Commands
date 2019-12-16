#ifndef __PARSER_H
#define __PARSER_H

//input: string from user to split into tokens readable by exec_args
//Returns a malloced block of memory, which should be freed when no longer needed
//Returns NULL if invalid format, and prints reason why
char** split_input(char *input);

//input: user-provided string from getline or fgets
//terminates string at first newline, excluding it
void strip_newline(char *input);

#endif
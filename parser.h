#ifndef __PARSER_H
#define __PARSER_H


//Returns a calloced block of memory
//Free when done
//Returns NULL if invalid format, and prints reason why
char** split_input(char *input);

//terminates string at first newline, excluding it
void strip_newline(char *input);

#endif
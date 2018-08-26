#ifndef OPENUTASK14_ALGORITHM_H
#define OPENUTASK14_ALGORITHM_H

#include "File.h"
#include "DataStructures.h"
#define MAX_NUM 512
#define PRINT "prn"
#define COMPARE "cmp"
#define RETURN_FROM_SUBROUTINE "rts"
#define STOP "stop"
#define LOAD_EFFECTIVE_ADDRESS "lea"
#define MOVE "mov"
#define MINUS '-'
#define PLUS '+'
#define ZERO '0'
#define NINE '9'
#define SPACE ' '
#define TAB '\t'
#define NULL_CHAR '\0'
#define NEW_LINE '\n'
#define HASH_TAG '#'
#define COMMA ','
#define OPEN_BRACKET '('
#define CLOSE_BRACKET ')'
#define JUMP "jmp"
#define BRANCH_IF_NOT_EQUAL "bne"
#define READ_MODE "r"
#define COMMENT_SIGN ';'
#define COLON ':'
#define DOT '.'
#define TRUE 1
#define FALSE 0


typedef enum CurrentRun {firstRun, secondRun} currentRun;

void ProcessDecNum(char *num, char *parameter, currentRun currRun, unsigned char address);
int skip_spaces(char *arr, int startingIndex);
int next_comma(char *arr, int startingIndex);
int is_variable(char *parameter);
int is_register(char *parameter);
int is_label(char *label);
int isSpace(char c);

#endif

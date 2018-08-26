#ifndef OPENUTASK14_READFROMFILE_H
#define OPENUTASK14_READFROMFILE_H

#include <stdio.h>

#define LINE_SIZE 80

/* Data structures */
typedef struct fileRepresentation {
    FILE *fd;
    char* fileName;
    char buffer[LINE_SIZE];
} fileRep;

/* Functions */
fileRep* OpenFile(char *fileName, char *mode);
void Read(fileRep *fileInfo);
void Write(fileRep *fileInfo);

/* Vars */
fileRep *file;


#endif

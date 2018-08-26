#include <stdlib.h>
#include "File.h"


fileRep* OpenFile(char *fileName, char *mode) {

    fileRep* file = (fileRep*)malloc(sizeof(fileRep));
    file->fd = fopen(fileName , mode);

    if(file->fd == NULL) {
        perror("Error opening file");
        return NULL;
    }

    file->fileName = fileName;

    return file;

}

void Read(fileRep *fileInfo) {
    if( fgets (fileInfo->buffer, LINE_SIZE, fileInfo->fd) == NULL && fileInfo->buffer[0] != '\0') {
        perror("Error reading from a file");
        exit(-1);
    }
}

void Write(fileRep *fileInfo) {
    if (fputs(fileInfo->buffer, fileInfo->fd) < 0) {
        perror("Error writing to a file");
        exit(-1);
    }
}
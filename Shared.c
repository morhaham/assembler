#include "Shared.h"

int isSpace(char c) {
    int i = 0;
    char whiteSpaces[7] = {' ', '\t', '\f', '\v', '\r', '\n', '\0'};
    while(whiteSpaces[i] != '\0') {
        if (c == whiteSpaces[i]) {
            return TRUE;
        }
        i++;
    }
    return FALSE;
}

int is_register(char *parameter) {
    if (parameter[0] != 'r') {
        return 0;
    }
    if (parameter[1] > '7' || parameter[1] < '0') {
        return 0;
    }
    if (parameter[2] != '\0') {
        return 0;
    }

    return 1;
}

int is_variable(char *parameter) {
    return is_label(parameter);

}

int is_label(char *label) {
    if (!(label[0] >= 'a' && label[0] <= 'z') && !(label[0] >= 'A' && label[0] <= 'Z')) {
        return 0;
    }

    while (*label != '\0') {
        if (!(*label >= 'a' && *label <= 'z') && !(*label >= 'A' && *label <= 'Z') &&
            !(*label >= '0' && *label <= '9')) {
            return 0;
        }
        label++;
    }

    return 1;
}

/* This function will add a decimal number to the commands DS*/
void ProcessDecNum(char *num, char *parameter, currentRun currRun, unsigned char address) {
    /*negative value solution*/
    char temp;
    unsigned short value;
    char * type;
    if (currRun == firstRun) {
        temp = (char)atoi(num);
        value = (unsigned short)temp;
    } else{
        value = address;
    }
    /*2 bits for ERA*/
    value = value << 2;

    if (currRun == firstRun) {
        ReallocOperationsArrByOne();
        operationsArr[IC] = value;
    } else{
        type = GetType(labelHT, parameter);

        if (type == NULL) {
            printf("%s:%d:%lu: error: label does not exist\n\t%s",file->fileName, LC,
                   parameter - file->buffer, file->buffer);
            secondRunError = 1;
            return;
        }
        if (strcmp(type, "extern") == 0) {
            ReallocExtEntDSByOne(&extDS, &extSize);
            strncpy(extDS[extSize - 1].labelName, parameter, LINE_SIZE);
            extDS[extSize - 1].address = (unsigned char)IC;
            value |= 1;
        } else if (strcmp(type, "entryData") == 0 || strcmp(type, "data") == 0){
            value = value >> 2;
            value += FRIC;
            value = value << 2;
            value |= 2;
        } else {
            value |= 2;
        }
        operationsArr[IC] = value;
    }
    IC++;
}

int skip_spaces(char *arr, int startingIndex) {
    int counter = 0;
    while (arr[startingIndex + counter] == SPACE || arr[startingIndex + counter] == TAB) {
        counter++;
    }
    return startingIndex + counter;
}

int next_comma(char *arr, int startingIndex){
    int counter = 0;
    while ((startingIndex+counter) != 79 && arr[startingIndex + counter] != ',') {
        counter++;
    }
    if ((startingIndex+counter) == 79) {
        return -1;
    }
    return startingIndex + counter;
}
#include "DataStructures.h"

void init() {
    extSize = 0;
    entSize = 0;
    firstRunError = 0;
    secondRunError = 0;
    operationsArr = NULL;
    dataArr = NULL;
    extDS = NULL;
    entDS = NULL;

    initBinaryArray();
    initCommandDS();
    InitializeCommandArgsDS();
    initRegisterDS();
    labelHT = CreateHashTable(65536);
    IC = 0;
    DC = 0;
    LC = 0;

    file = NULL;
}


void initBinaryArray() {
    binaryCharArr[0] = '.';
    binaryCharArr[1] = '/';
}

void initCommandDS() {
    commandHT = CreateHashTable(17);
    SetValue(commandHT, "mov", 0, NULL);
    SetValue(commandHT, "cmp", 1, NULL);
    SetValue(commandHT, "add", 2, NULL);
    SetValue(commandHT, "sub", 3, NULL);
    SetValue(commandHT, "not", 4, NULL);
    SetValue(commandHT, "clr", 5, NULL);
    SetValue(commandHT, "lea", 6, NULL);
    SetValue(commandHT, "inc", 7, NULL);
    SetValue(commandHT, "dec", 8, NULL);
    SetValue(commandHT, "jmp", 9, NULL);
    SetValue(commandHT, "bne", 10, NULL);
    SetValue(commandHT, "red", 11, NULL);
    SetValue(commandHT, "prn", 12, NULL);
    SetValue(commandHT, "jsr", 13, NULL);
    SetValue(commandHT, "rts", 14, NULL);
    SetValue(commandHT, "stop",15, NULL);
}

void initRegisterDS() {
    registerHT = CreateHashTable(11);
    SetValue(registerHT, "r0", 0, NULL);
    SetValue(registerHT, "r1", 1, NULL);
    SetValue(registerHT, "r2", 2, NULL);
    SetValue(registerHT, "r3", 3, NULL);
    SetValue(registerHT, "r4", 4, NULL);
    SetValue(registerHT, "r5", 5, NULL);
    SetValue(registerHT, "r6", 6, NULL);
    SetValue(registerHT, "r7", 7, NULL);
}

void ReallocDataArrByOne() {
    if (dataArr == NULL) {
        dataArr = (AllData*)malloc(sizeof(AllData));
        if (dataArr == NULL) {
            exit(-1);
        }
        return;
    }

    dataArr = (AllData*)realloc(dataArr, (DC + 1)* sizeof(AllData));

    if (dataArr == NULL) {
        exit(-1);
    }
}

void ReallocOperationsArrByOne() {
    if (operationsArr == NULL) {
        operationsArr = (unsigned short*)malloc(sizeof(short));
        if (operationsArr == NULL) {
            exit(-1);
        }
        return;
    }

    operationsArr =
            (unsigned short*)realloc(operationsArr, (IC + 1)* sizeof(short));

    if (operationsArr == NULL) {
        exit(-1);
    }
}

void ReallocExtEntDSByOne(extOrEntTable **extendTable, int *currentTableSize) {
    if (*extendTable == NULL) {
        *extendTable = (extOrEntTable*)malloc(sizeof(extOrEntTable));
        if (*extendTable == NULL) {
            exit(-1);
        }
        (*currentTableSize)++;
        return;
    }

    *extendTable = (extOrEntTable*)realloc(*extendTable, (*currentTableSize + 1)* sizeof(extOrEntTable));

    if (*extendTable == NULL) {
        exit(-1);
    }

    (*currentTableSize)++;
}

void finalise() {
    free(operationsArr);
    free(dataArr);
    free(extDS);
    free(entDS);
    if (file != NULL) {
        if (file->fd != NULL) {
            fclose(file->fd);
        }
        free(file);
    }
    DeleteHashTable(&labelHT);
    DeleteHashTable(&commandHT);
    DeleteHashTable(&commandNumOfArgs);
    DeleteHashTable(&registerHT);
}

void InitializeCommandArgsDS() {
    commandNumOfArgs = CreateHashTable(17);
    SetValue(commandNumOfArgs, "mov", 2, NULL);
    SetValue(commandNumOfArgs, "cmp", 2, NULL);
    SetValue(commandNumOfArgs, "add", 2, NULL);
    SetValue(commandNumOfArgs, "sub", 2, NULL);
    SetValue(commandNumOfArgs, "not", 1, NULL);
    SetValue(commandNumOfArgs, "clr", 1, NULL);
    SetValue(commandNumOfArgs, "lea", 2, NULL);
    SetValue(commandNumOfArgs, "inc", 1, NULL);
    SetValue(commandNumOfArgs, "dec", 1, NULL);
    SetValue(commandNumOfArgs, "jmp", 1, NULL);
    SetValue(commandNumOfArgs, "bne", 1, NULL);
    SetValue(commandNumOfArgs, "red", 1, NULL);
    SetValue(commandNumOfArgs, "prn", 1, NULL);
    SetValue(commandNumOfArgs, "jsr", 1, NULL);
    SetValue(commandNumOfArgs, "rts", 0, NULL);
    SetValue(commandNumOfArgs, "stop",0, NULL);
}
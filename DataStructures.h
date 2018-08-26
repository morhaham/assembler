#ifndef OPENUTASK14_ASSEMBLYRELATIVEDS_H
#define OPENUTASK14_ASSEMBLYRELATIVEDS_H

#include "HashTabel.h"
#include "File.h"

#define LINE_SIZE 80
#define OUTPUT_BASE 2
typedef enum DataType {data, string} dataType;

/*data types known by the assembler (.data, .string)*/
typedef struct AllData {
    char charData;
    int integerData;
} AllData;

typedef struct ExternOrEntryAddressTable {
    char labelName[LINE_SIZE];
    unsigned char address;
} extOrEntTable;

/*Instruction counter, data counter, line counter and first run instruction counter*/
int IC;
int DC;
int LC;
int FRIC;

/*external and entry DS counters*/
int extSize;
int entSize;

int firstRunError;
int secondRunError;

/*data structures.*/
/*label name to address mapping*/
hashTable* labelHT;
/*command name to binary code mapping*/
hashTable* commandHT;
hashTable* commandNumOfArgs;
hashTable* registerHT;
/*DS to store all the data*/
AllData* dataArr;
extOrEntTable* extDS;
extOrEntTable* entDS;

/*DS to store each character of the 2 base "imaginary" computer*/
char binaryCharArr[OUTPUT_BASE];
/*There just enough space for storing a word in "imaginary" machine that consisted of 14 bits*/
unsigned short* operationsArr;

fileRep* file;

void init();
void initBinaryArray();
void initCommandDS();
void initRegisterDS();
void InitializeCommandArgsDS();
void finalise();
void ReallocOperationsArrByOne();
void ReallocDataArrByOne();
void ReallocExtEntDSByOne(extOrEntTable **extendTable, int *currentTableSize);

#endif

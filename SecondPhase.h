#ifndef OPENUTASK14_2018_SECONDSCANMANIPULATION_H
#define OPENUTASK14_2018_SECONDSCANMANIPULATION_H

#define DEC_ADDRESS_MAX_NUM_OF_DIGITS 4

#define OB_FILE_EXTENSION ".ob"
#define DOT_DELIMITER "."
#define WRITE_MODE "w"
#define ENT_FILE_EXTENSION ".ent"
#define EXT_FILE_EXTENSION ".ext"

typedef enum ConversionType {DATA_CT, INSTRUCTION_CT} CT;

void PerformSecondScan(char *fileName);

#endif

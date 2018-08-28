#include "SecondPhase.h"
#include "DataStructures.h"
#include "Shared.h"

void CreateOutputFiles(char *fileName);
void SecondRunCommandProcess(char *arr, int firstIndex, char *commandName);
void SecondRunProcessCommandParam(char *parameter, int registerFlag);
void ConvertToBinaryForm(CT type, int numOfElements);
void SaveToExtAndEntFiles(fileRep** file, extOrEntTable* table, int tableSize);

/*This function will create the output files*/
void CreateOutputFiles(char *fileName) {
    /*create a file with ".ob" extension for writing*/
    char newFileName[LINE_SIZE];
    char * delimiterLocation = strtok(fileName, DOT_DELIMITER);

    memset(newFileName, '\0', LINE_SIZE);

    strncpy(newFileName, delimiterLocation, LINE_SIZE);
    strcat(newFileName, OB_FILE_EXTENSION);

    file = OpenFile(newFileName, WRITE_MODE);

    ConvertToBinaryForm(INSTRUCTION_CT, IC);
    ConvertToBinaryForm(DATA_CT, DC);

    fclose(file->fd);
    file->fd = NULL;

    /*create a file with ".ent" extension for writing*/
    memset(newFileName, '\0', LINE_SIZE);
    strncpy(newFileName, delimiterLocation, LINE_SIZE);
    strcat(newFileName, ENT_FILE_EXTENSION);
    file = OpenFile(newFileName, WRITE_MODE);
    SaveToExtAndEntFiles(&file, entDS, entSize);

    fclose(file->fd);
    file->fd = NULL;

    /*create a file with ".ext" extension for writing*/
    memset(newFileName, '\0', LINE_SIZE);
    strncpy(newFileName, delimiterLocation, LINE_SIZE);
    strcat(newFileName, EXT_FILE_EXTENSION);
    file = OpenFile(newFileName, WRITE_MODE);
    SaveToExtAndEntFiles(&file, extDS, extSize);
}
/* This function will update the address value for parameters that are variables only*/
void SecondRunProcessCommandParam(char *parameter, int registerFlag) {
    unsigned char value;
    if (parameter[0] == HASH_TAG) {
        IC++;
        return;
    } else if (is_register(parameter)) {
        if (registerFlag == 1) {
            return;
        } else{
            IC++;
            return;
        }
    } else if (is_variable(parameter)) {
        value = GetValue(labelHT, parameter);
        ProcessDecNum(NULL, parameter, secondRun, value);
    }
}

void SecondRunCommandProcess(char *arr, int firstIndex, char *commandName) {
    int nextIndex = firstIndex, isFirstParam = 1, isRegister = 0, jmpOrBneWithArgs = 0;
    char temp[LINE_SIZE];
    IC++;

    if (strcmp(commandName, RETURN_FROM_SUBROUTINE) == 0 || strcmp(commandName, STOP) == 0) {
        return;
    }

    /*we will skip all the whitespaces to find the first occurrence of the parameter value*/
    nextIndex = skip_spaces(arr, nextIndex);
    firstIndex = nextIndex;

    /*keep processing the next element until there no more left (returned -1)*/
    while (nextIndex != -1 && nextIndex != LINE_SIZE) {
        if (arr[nextIndex] != SPACE && arr[nextIndex] != TAB &&
            arr[nextIndex] != COMMA && arr[nextIndex] != NEW_LINE
            && arr[nextIndex] != OPEN_BRACKET && arr[nextIndex] != CLOSE_BRACKET){
            nextIndex++;
            continue;
        }

        memset(temp, '\0', LINE_SIZE);

        strncpy(temp, &arr[firstIndex],
                nextIndex - 1 - firstIndex + 1);
        /*this is when we will call the SecondRunProcessCommandParam function to update all the missing info
        from the first run (label addresses as they found in the first run and saved in appropriate DS)*/
        if (isFirstParam == 1) {
            isRegister = is_register(temp);
            SecondRunProcessCommandParam(temp, 0);
            isFirstParam = 0;
        } else if (jmpOrBneWithArgs == 1) {
            isRegister = is_register(temp);
            SecondRunProcessCommandParam(temp, 0);
            jmpOrBneWithArgs = 0;
        }else {
            SecondRunProcessCommandParam(temp, isRegister);
        }

        if ((arr[skip_spaces(arr, nextIndex)] == OPEN_BRACKET
             || arr[skip_spaces(arr, nextIndex)] == CLOSE_BRACKET)
            && (strcmp(commandName, JUMP) == 0 || strcmp(commandName, BRANCH_IF_NOT_EQUAL) == 0)) {

            if (arr[skip_spaces(arr, nextIndex)] == CLOSE_BRACKET) {
                break;
            }

            jmpOrBneWithArgs = 1;
            nextIndex = skip_spaces(arr, nextIndex);
            nextIndex++;
            nextIndex = skip_spaces(arr, nextIndex);
            firstIndex = nextIndex;
            continue;
        }
        /*we will skip all the whitespaces to find the next occurrence of the parameter value*/
        nextIndex = next_comma(arr, nextIndex);
        if (nextIndex == -1) {
            break;
        }
        nextIndex = skip_spaces(arr, nextIndex + 1);
        firstIndex = nextIndex;
    }
}

void PerformSecondScan(char *fileName) {
    IC = 0;
    file = OpenFile(fileName, READ_MODE);
    Read(file);
    /*scan the lines*/
    while(file->buffer[0] != '\0') {
        int i, firstIndex = 0, foundWord = 0;
        char commandString[LINE_SIZE];
        /*scan the line characters*/
        for (i = 0; i < LINE_SIZE; i++) {
            /*end of line reached*/
            if (file->buffer[i] == NEW_LINE && foundWord == 0) {
                break;
            }
            if (file->buffer[i] != SPACE && file->buffer[i] != TAB&&
                file->buffer[i] != NEW_LINE) {

                /*check for comment*/
                if (file->buffer[i] == COMMENT_SIGN) {
                    break;
                }

                if (!foundWord) {
                    firstIndex = i;
                }
                foundWord = 1;

            } else if (file->buffer[i] == SPACE || file->buffer[i] == TAB ||
                       file->buffer[i] == NEW_LINE) {

                if (foundWord == 1) {
                    if (file->buffer[i - 1] == COLON) {
                        foundWord = 0;
                        continue;
                    }
                    else if (file->buffer[firstIndex] == DOT) {
                        break;
                        /*found a command statement*/
                    } else {
                        memset(commandString, '\0', LINE_SIZE);
                        strncpy(commandString, &file->buffer[firstIndex],
                                i - 1 - firstIndex + 1);
                        SecondRunCommandProcess(file->buffer, i, commandString);
                        break;
                    }
                }
                foundWord = 0;
            }

        }
        memset(file->buffer, '\0', LINE_SIZE);
        /*read the next line from file*/
        Read(file);
    }

    fclose(file->fd);
    file->fd = NULL;
    if (!secondRunError) {
        CreateOutputFiles(fileName);
    }
    FRIC = 0;
}

void ConvertToBinaryForm(CT type, int numOfElements) {
    int i, j;
    unsigned short address;
    unsigned short binaryCode;
    unsigned short mask;
    char reminder;

    /*iterate over all the command words*/
    for (i = 0; i < numOfElements; i++) {
        if (type == DATA_CT) {
            /*continue from the address of the last command*/
            address = (unsigned short)(i + IC);
            /*check the current data type*/
            if (dataArr[i].integerData != MAX_NUM + 1) {
                binaryCode = (unsigned short)dataArr[i].integerData;
            } else if (dataArr[i].charData != '\0') {
                binaryCode = (unsigned short)dataArr[i].charData;
            }
        } else {
            address = (unsigned short) i;
            binaryCode = operationsArr[i];
        }
        memset(file->buffer, '\0', LINE_SIZE);

        for (j = 0; j < DEC_ADDRESS_MAX_NUM_OF_DIGITS; j++) {
            file->buffer[j] = ZERO;
        }
        file->buffer[j--] = TAB;

        while (address != 0) {
            reminder = (char)(address%10);
            file->buffer[j] = (char)(reminder + 48);
            address /= 10;
            j--;
        }

        mask = 8192;

        for(j = DEC_ADDRESS_MAX_NUM_OF_DIGITS + 1; mask > 0; j++) {
            if (mask&binaryCode) {
                file->buffer[j] = binaryCharArr[1];
            } else {
                file->buffer[j] = binaryCharArr[0];
            }
            mask = mask >> 1;
        }

        file->buffer[j] = NEW_LINE;

        Write(file);
    }
}

void SaveToExtAndEntFiles(fileRep** file, extOrEntTable* table, int tableSize) {
    int i;
    char convertedNumToStr[3];

    for (i = 0; i < tableSize; i++) {
        memset((*file)->buffer, NULL_CHAR, LINE_SIZE);
        strncpy((*file)->buffer, table[i].labelName, LINE_SIZE);
        strcat((*file)->buffer, "\t");
        sprintf(convertedNumToStr, "%d", table[i].address);
        strcat((*file)->buffer, convertedNumToStr);
        strcat((*file)->buffer, "\n");
        Write(*file);
    }
}
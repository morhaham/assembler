#include "FirstScan.h"
#include "DataStructures.h"
#include "Shared.h"

int next_char(char *arr, int startIndex);
void ProcessCommand(char *arr, int firstIndex, char *commandName);
unsigned short ProcessCommandParam(char *parameterString, int registerFlag, int isFirstRegister);
int is_constant(char *parameter);
void DataProcess(char *dataToProcess, int startIndex, dataType dataType);
int is_command(char *label);
int is_matching_operand(unsigned short dest, unsigned short origin, char* command);
int HandleFoundWord(int currentIndex, int firstDataIndexFound, int* labelFoundFlag, char* labelTempString);
int HandleDataCase(char* labelString, char* temp, int currentIndex);
int HandleEntAndExtInstructionCase(int currentIndex, char* temp);

int is_matching_operand(unsigned short dest, unsigned short origin, char* command) {

    if (strcmp(command, PRINT) != 0 && strcmp(command, COMPARE) != 0 && strcmp(command, RETURN_FROM_SUBROUTINE) != 0 &&
        strcmp(command, STOP) != 0) {

        if (strcmp(command, LOAD_EFFECTIVE_ADDRESS) == 0) {
            if (origin != 1) {
                return 0;
            }
        }

        if (dest == 0) {
            return 0;
        }
    }

    return 1;
}

int next_char(char *arr, int startIndex) {
    if (startIndex != LINE_SIZE - 1 && arr[startIndex + 1] != '"') {
        return startIndex + 1;
    }
    /*end of line reached no closing quotation mark found*/
    if (startIndex == LINE_SIZE - 1) {
        return -2;
    }
    return -1;
}
int is_command(char *label) {

    if (strcmp(label, MOVE) == 0) {
        return 1;
    }

    return GetValue(commandHT, label);
}

int is_constant(char *parameter) {
    if (parameter[0] != MINUS && parameter[0] != PLUS
        && !(parameter[0] >=ZERO && parameter[0] <= NINE)) {
        return 0;
    }

    parameter++;

    while (*parameter != NULL_CHAR) {
        if (!(*parameter >= ZERO && *parameter <= NINE)) {
            return 0;
        }
        parameter++;
    }
    return 1;
}

/*this function will check if there white spaces between digits of constant data type*/
int is_data_type_constant(char *data) {
    int firstIndex = 0;
    int lastIndex = 0;
    char temp[LINE_SIZE];
    firstIndex = skip_spaces(data, firstIndex);
    if (firstIndex == -1) {
        printf("%s:%d:%d: error: no data type constant\n\t%s",file->fileName, LC, lastIndex, file->buffer);
        return 0;
    }
    lastIndex = firstIndex;
    while (data[lastIndex] != SPACE && data[lastIndex] != TAB &&
           data[lastIndex] != NULL_CHAR && data[lastIndex] != NEW_LINE){
        lastIndex++;
    }

    memset(temp, NULL_CHAR, LINE_SIZE);
    if (data[lastIndex] == SPACE || data[lastIndex] == TAB ||
        data[lastIndex] == NEW_LINE) {
        strncpy(temp, &data[firstIndex], lastIndex - 1 - firstIndex + 1);
    } else{
        strncpy(temp, &data[firstIndex], lastIndex - firstIndex + 1);
    }

    lastIndex = skip_spaces(data, lastIndex);
    if (!is_constant(temp) || (data[lastIndex] != NULL_CHAR && data[lastIndex] != NEW_LINE)) {
        printf("%s:%d:%d: error: wrong data type constant\n\t%s",file->fileName, LC, lastIndex, file->buffer);
        return 0;
    }

    return 1;
}

/*This function will extract the data and save it to the appropriate data structure*/
void DataProcess(char *dataToProcess, int startIndex, dataType dataType) {
    int nextIndex = startIndex;
    int integerData;
    char temp[LINE_SIZE];
    /*process next data element*/
    while (nextIndex != -1) {
        if (dataType == data) {
            memset(temp, NULL_CHAR, LINE_SIZE);
            if (next_comma(dataToProcess, nextIndex) != -1) {
                strncpy(temp, &dataToProcess[nextIndex], next_comma(dataToProcess, nextIndex) - 1
                                                         - nextIndex + 1);
            } else {
                strncpy(temp, &dataToProcess[nextIndex], LINE_SIZE - 1
                                                         - nextIndex + 1);
            }
            if (!is_data_type_constant(temp)) {
                firstRunError = 1;
                return;
            }
            /*get the next number from the sequence of type 'data'*/
            integerData = atoi(&dataToProcess[nextIndex]);
            ReallocDataArrByOne();
            dataArr[DC].integerData = integerData;
            dataArr[DC].charData = NULL_CHAR;
            DC++;
            nextIndex = next_comma(dataToProcess, nextIndex);
            if (nextIndex == -1) {
                break;
            }
            nextIndex++;
        }
        else if (dataType == string) {
            ReallocDataArrByOne();
            dataArr[DC].integerData = MAX_NUM + 1;
            dataArr[DC].charData = dataToProcess[nextIndex];
            DC++;
            nextIndex = next_char(dataToProcess, nextIndex);
            /*end of the string*/
            if (nextIndex == -1) {
                ReallocDataArrByOne();
                dataArr[DC].integerData = MAX_NUM + 1;
                dataArr[DC].charData = NULL_CHAR;
                DC++;
            } else if (nextIndex == -2){
                firstRunError = 1;
                printf("%s:%d:%d: error: wrong string data type value\n\t%s",file->fileName, LC, nextIndex, file->buffer);
            }
        }
    }
}

/* This function will process parameters by saving them as words in "imaginary" format machine*/
unsigned short ProcessCommandParam(char *parameterString, int registerFlag, int isFirstRegister) {
    unsigned short currentWord;
    char registerCode;

    if (parameterString[0] == HASH_TAG) {
        if (!is_constant(parameterString + 1)) {
            printf("%s:%d:%lu: error: wrong constant variable value\n\t%s",file->fileName, LC,
                   parameterString - file->buffer, file->buffer);
        }
        ProcessDecNum(parameterString + 1, NULL, firstRun, 0);
        return CONSTANT_CODE;
    } else if (is_register(parameterString)) {
        /*second register, there no need to allocate more space*/
        if (registerFlag == 1) {
            currentWord = operationsArr[IC - 1];
            registerCode = GetValue(registerHT, parameterString);
            registerCode <<= 2;
            currentWord = currentWord | (unsigned short)registerCode;
            operationsArr[IC - 1] = currentWord;
            /*no register found as a previous parameter*/
        } else{
            ReallocOperationsArrByOne();
            /*register as the first parameter*/
            if (isFirstRegister) {
                registerCode = GetValue(registerHT, parameterString);
                currentWord = (unsigned short)registerCode<<8;
                operationsArr[IC] = currentWord;
                /*register as the second parameter*/
            } else{
                registerCode = GetValue(registerHT, parameterString);
                currentWord = (unsigned short)registerCode<<2;
                operationsArr[IC] = currentWord;
            }
            IC++;
        }
        return REGISTER_CODE;
    } else if (is_variable(parameterString)) {
        ReallocOperationsArrByOne();

        /*the address currently set to 0, will be updated on the second run*/
        operationsArr[IC] = 0;
        IC++;
        return VARIABLE_CODE;
    } else{
        printf("%s:%d:%lu: error: wrong command parameter\n\t%s",file->fileName, LC,
               parameterString - file->buffer, file->buffer);
        firstRunError = 1;
    }
    return ERROR;
}

/*This function will process a command and its parameters*/
void ProcessCommand(char *arr, int firstIndex, char *commandName) {
    int nextIndex = firstIndex, commandIC = IC, isRegister, paramCount = 0, jmpOrBneWithArgs = 0;
    char commandCode = GetValue(commandHT, commandName), temp[LINE_SIZE];
    unsigned short dest = 0, origin = 0, jmpOrBne = 2, commandWord;
    unsigned char commandMaxArgs = GetValue(commandNumOfArgs, commandName);
    ReallocOperationsArrByOne();
    IC++;

    /*check if for commands that do not take any arguments we reach end of line*/
    if (strcmp(commandName, RETURN_FROM_SUBROUTINE) == 0 || strcmp(commandName, STOP) == 0) {
        if (arr[skip_spaces(arr, nextIndex)] != NEW_LINE) {
            printf("%s:%d:%d: error: rts and stop command do not take arguments\n\t%s",
                   file->fileName, LC, nextIndex, file->buffer);
            firstRunError = 1;
            return;
        }
    } else {

        /*skip all the whitespaces to find the first parameter value*/
        nextIndex = skip_spaces(arr, nextIndex);
        firstIndex = nextIndex;

        if (arr[nextIndex] == NEW_LINE) {
            printf("%s:%d:%d: error: command requires at least one argument\n\t%s",
                   file->fileName, LC, nextIndex, file->buffer);
            firstRunError = 1;
            return;
        }

        /*process the next element*/
        while (nextIndex != -1 && nextIndex != LINE_SIZE) {
            if (arr[nextIndex] != SPACE && arr[nextIndex] != TAB &&
                arr[nextIndex] != COMMA && arr[nextIndex] != NEW_LINE
                && arr[nextIndex] != OPEN_BRACKET && arr[nextIndex] != CLOSE_BRACKET) {
                nextIndex++;
                continue;
            }

            memset(temp, NULL_CHAR, LINE_SIZE);

            strncpy(temp, &arr[firstIndex],
                    nextIndex - 1 - firstIndex + 1);

            /*process parameters*/
            if (paramCount == 0) {
                isRegister = is_register(temp);
                origin = ProcessCommandParam(temp, 0, 1);
                paramCount++;
            }
            else if (jmpOrBneWithArgs == 1 && paramCount == 1) {
                isRegister = is_register(temp);
                origin = ProcessCommandParam(temp, 0, 1);
                paramCount++;
            }
            else {
                dest = ProcessCommandParam(temp, isRegister, 0);
                paramCount++;
            }

            if ((arr[skip_spaces(arr, nextIndex)] == OPEN_BRACKET
                 || arr[skip_spaces(arr, nextIndex)] == CLOSE_BRACKET)
                && (strcmp(commandName, JUMP) == 0 || strcmp(commandName, BRANCH_IF_NOT_EQUAL) == 0)) {

                if (arr[skip_spaces(arr, nextIndex)] == CLOSE_BRACKET) {
                    nextIndex = skip_spaces(arr, nextIndex);
                    nextIndex++;
                    if (arr[skip_spaces(arr, nextIndex)] != NEW_LINE) {
                        firstRunError = 1;
                        printf("%s:%d:%d: error: wrong command argument\n\t%s",
                               file->fileName, LC, nextIndex, file->buffer);
                    }
                    break;
                }

                jmpOrBneWithArgs = 1;
                commandMaxArgs = 3;
                nextIndex = skip_spaces(arr, nextIndex);
                nextIndex++;
                nextIndex = skip_spaces(arr, nextIndex);
                firstIndex = nextIndex;
                continue;
            }

            if (arr[skip_spaces(arr, nextIndex)] != COMMA &&
                arr[skip_spaces(arr, nextIndex)] != NEW_LINE) {
                firstRunError = 1;
                printf("%s:%d:%d: error: wrong command argument\n\t%s",
                       file->fileName, LC, nextIndex, file->buffer);
            }

            /*command has more than maximum two possible parameters*/
            if (paramCount > commandMaxArgs) {
                printf("%s:%d:%d: error: too much arguments, this command takes %d arguments\n\t%s",
                       file->fileName, LC, nextIndex, commandMaxArgs, file->buffer);
                firstRunError = 1;
                return;
            }
            /*skip all the whitespaces to find the next parameter value*/
            nextIndex = next_comma(arr, nextIndex);
            if (nextIndex == -1) {
                break;
            }
            nextIndex = skip_spaces(arr, nextIndex + 1);
            firstIndex = nextIndex;
        }
    }

    if (paramCount != commandMaxArgs) {
        printf("%s:%d:%d: error: not enough arguments, this command takes %d arguments\n\t%s",
               file->fileName, LC, nextIndex, commandMaxArgs, file->buffer);
        firstRunError = 1;
        return;
    }

    /*no origin container for unary function*/
    if (paramCount == 1) {
        dest = origin;
        origin = 0;
    }

    if (!is_matching_operand(dest, origin, commandName)) {
        printf("%s:%d:%d: error: wrong operand\n\t%s",
               file->fileName, LC, nextIndex, file->buffer);
        firstRunError = 1;
        return;
    }

    /*construct the main command operation word*/
    commandWord = (unsigned short)commandCode<<6;
    if (jmpOrBneWithArgs) {
        jmpOrBne <<= 2;
        dest <<= 10;
        origin <<= 12;
        commandWord = commandWord | dest | origin | jmpOrBne;
    } else {
        dest <<= 2;
        origin <<= 4;
        commandWord = commandWord | dest | origin;
    }
    operationsArr[commandIC] = commandWord;
}
/*
 * This function will perform the first scan of the input file and update the appropriate DS in the process
 */
void PerformFirstScan(char *fileName) {
    int i;
    char labelString[LINE_SIZE];
    file = OpenFile(fileName, READ_MODE);
    Read(file);
    /*scan the whole file*/
    while(file->buffer[0] != NULL_CHAR) {
        int firstIndex = 0;
        int wordFound = 0, labelFound = 0;
        LC++;
        /*scan the current line*/
        for (i = 0; i < LINE_SIZE; i++) {
            /*end of line*/
            if (file->buffer[i] == NEW_LINE && wordFound == 0) {
                break;
            }
            /*found not a white space*/
            if (!isSpace(file->buffer[i])) {

                /*comment found*/
                if (file->buffer[i] == COMMENT_SIGN && wordFound == 0) {
                    break;
                }

                if (!wordFound) {
                    /*save the starting index of the word*/
                    firstIndex = i;
                }
                wordFound = 1;
            } else if (isSpace(file->buffer[i])) {
                /*case we found a word, check and process it*/
                if (wordFound == 1) {
                    if (!HandleFoundWord(i, firstIndex, &labelFound, labelString)) {
                        break;
                    }
                }
                wordFound = 0;
            }

        }
        memset(file->buffer, NULL_CHAR, LINE_SIZE);
        /*read the next line from file*/
        Read(file);
    }
    FRIC = IC;
    for (i = 0; i < entSize; i++) {
        if (strcmp(GetType(labelHT, entDS[i].labelName), "entryData") == 0) {
            entDS[i].address += FRIC;
        }
    }

    fclose(file->fd);
    file->fd = NULL;
}


int HandleFoundWord(int currentIndex, int firstDataIndexFound, int* labelFoundFlag, char* labelTempString) {

    char tempString[LINE_SIZE], commandTempString[LINE_SIZE];
    /*case we found a label*/
    if (file->buffer[currentIndex - 1] == COLON) {
        /*save the label name*/
        memset(labelTempString, NULL_CHAR, LINE_SIZE);
        strncpy(labelTempString, &file->buffer[firstDataIndexFound],
                currentIndex - 2 - firstDataIndexFound + 1);
        if (!is_label(labelTempString)) {
            firstRunError = 1;
            printf("%s:%d:%d: error: wrong label format\n\t%s",file->fileName, LC, currentIndex, file->buffer);
            return 0;
        }

        *labelFoundFlag = 1;
        return 1;
    }
        /*two possible cases: found data or entry/extern*/
    else if (file->buffer[firstDataIndexFound] == DOT) {

        memset(tempString, NULL_CHAR, LINE_SIZE);
        strncpy(tempString, &file->buffer[firstDataIndexFound + 1],
                currentIndex - 1 - firstDataIndexFound);
        /*first case*/
        if (*labelFoundFlag == 1){

            if(HandleDataCase(labelTempString, tempString, currentIndex)){
                return 1;
            }
            *labelFoundFlag = 0;
            return 0;
            /*second case*/
        } else{
            if (HandleEntAndExtInstructionCase(currentIndex, tempString)) {
                return 1;
            }
            return 0;
        }
        /*found a command statement*/
    } else {
        /*check if command label, save the label with its address*/
        if (*labelFoundFlag == 1) {
            if (GetType(labelHT, labelTempString) == NULL) {
                SetValue(labelHT, labelTempString, (unsigned char)IC, "command");
            } else {
                if (strcmp(GetType(labelHT, labelTempString), "entry") == 0) {
                    SetValue(labelHT, labelTempString, (unsigned char)IC, "entryCommand");
                } else{
                    SetValue(labelHT, labelTempString, (unsigned char)IC, "externCommand");
                }
                ReallocExtEntDSByOne(&entDS, &entSize);
                strncpy(entDS[entSize - 1].labelName, labelTempString, LINE_SIZE);
                entDS[entSize - 1].address = (unsigned char)IC;
            }
            *labelFoundFlag = 0;
        }

        memset(commandTempString, NULL_CHAR, LINE_SIZE);
        strncpy(commandTempString, &file->buffer[firstDataIndexFound],
                currentIndex - 1 - firstDataIndexFound + 1);

        if (!is_command(commandTempString)) {
            firstRunError = 1;
            printf("%s:%d:%d: error: unknown command\n\t%s",file->fileName, LC, currentIndex, file->buffer);
            return 0;
        }

        ProcessCommand(file->buffer, currentIndex, commandTempString);
        return 0;
    }
}

int HandleDataCase(char* labelString, char* temp, int currentIndex) {
    /*save the data label with its address*/
    if (GetType(labelHT, labelString) == NULL) {
        SetValue(labelHT, labelString, (unsigned char)DC, "data");
    } else {
        if (strcmp(GetType(labelHT, labelString), "entry") == 0) {
            SetValue(labelHT, labelString, (unsigned char)DC, "entryData");
        } else{
            SetValue(labelHT, labelString, (unsigned char)DC, "externData");
        }
        ReallocExtEntDSByOne(&entDS, &entSize);
        strncpy(entDS[entSize - 1].labelName, labelString, LINE_SIZE);
        entDS[entSize - 1].address = (unsigned char)DC;
    }

    if (strcmp(temp, "data") == 0) {
        DataProcess(file->buffer,
                    skip_spaces(file->buffer, currentIndex), data);
    } else if (strcmp(temp, "string") == 0) {
        if (file->buffer[skip_spaces(file->buffer, currentIndex)] != '"') {
            firstRunError = 1;
            printf("%s:%d:%d: error: wrong string data type\n\t%s",file->fileName, LC, currentIndex, file->buffer);
            return 0;
        }
        DataProcess(file->buffer,
                    skip_spaces(file->buffer, currentIndex) + 1, string);
    }
    else{
        firstRunError = 1;
        printf("%s:%d:%d: error: wrong data type\n\t%s",file->fileName, LC, currentIndex, file->buffer);
    }
    return 0;
}

int HandleEntAndExtInstructionCase(int currentIndex, char* temp){
    int firstIndex, secondIndex;
    char labelString[LINE_SIZE];
    firstIndex = skip_spaces(file->buffer, currentIndex);
    secondIndex = firstIndex;

    while (file->buffer[secondIndex]!= SPACE &&
           file->buffer[secondIndex] != TAB &&
           file->buffer[secondIndex] != NEW_LINE) {
        secondIndex++;
    }

    if (file->buffer[skip_spaces(file->buffer, secondIndex)] != NEW_LINE) {
        firstRunError = 1;
        printf("%s:%d:%d: error: wrong label format\n\t%s",file->fileName, LC, currentIndex, file->buffer);
        return 0;
    }

    memset(labelString, NULL_CHAR, LINE_SIZE);
    strncpy(labelString, &file->buffer[firstIndex],
            secondIndex - 1 - firstIndex + 1);

    if (!is_label(labelString)) {
        firstRunError = 1;
        printf("%s:%d:%d: error: wrong label format\n\t%s",file->fileName, LC, currentIndex, file->buffer);
        return 0;
    }
    if (strcmp(temp, "entry") == 0) {
        if (GetType(labelHT, labelString) != NULL) {
            ReallocExtEntDSByOne(&entDS, &entSize);
            strncpy(entDS[entSize - 1].labelName, labelString, LINE_SIZE);
            entDS[entSize - 1].address = GetValue(labelHT, labelString);
            if (strcmp(GetType(labelHT, labelString), "data") == 0) {
                SetValue(labelHT, labelString, GetValue(labelHT, labelString), "entryData");
            } else{
                SetValue(labelHT, labelString, GetValue(labelHT, labelString), "entryCommand");
            }
        } else {
            SetValue(labelHT, labelString, 0, "entry");
        }
    } else if (strcmp(temp, "extern") == 0) {
        SetValue(labelHT, labelString, 0, "extern");
    } else{
        printf("%s:%d:%d: error: wrong instruction statement\n\t%s",file->fileName, LC, currentIndex, file->buffer);
        firstRunError = 1;
    }
    return 0;
}
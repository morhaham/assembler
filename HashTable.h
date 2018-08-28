#ifndef OPENUTASK14_LABELHASHTABEL_H
#define OPENUTASK14_LABELHASHTABEL_H

#define _XOPEN_SOURCE 500 /* Enable certain library functions (strdup) on linux.  See feature_test_macros(7) */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#define TYPE_SIZE 20

typedef struct entry {
    char *key;
    unsigned char value;
    char entryType[TYPE_SIZE];
    struct entry *next;
} entry;

typedef struct HashTable {
    int size;
    entry** table;
} hashTable;


hashTable *CreateHashTable( int size );

/*map the key to the value*/
void SetValue (hashTable* labelHashTable, char* key, unsigned char value,  char* entryType);
int hash(hashTable *hashtable, char *key);
entry *CreateNewPair( char *key, unsigned char value, char* entryType );
unsigned char GetValue( hashTable *hashtable, char *key );
char* GetType( hashTable *hashtable, char *key );
void DeleteHashTable(hashTable **hashTablel);

#endif

#include <stdio.h>
#include "DataStructures.h"
#include "FirstScan.h"
#include "SecondScan.h"


int main(int argc, char *argv[]) {

    int i;
    /*this function will initialize all the required data structures*/
    init();

    /*perform both the assembly runs one after another for each of the files supplied by the user*/
    for (i = 1; i < argc; i++) {
        PerformFirstScan(argv[i]);
        /*if there errors in the first scan there no need for the second scan*/
        if (!firstRunError) {
            PerformSecondScan(argv[i]);
        }
        /*clear all the allocated space on the heap*/
        finalise();
        if (i != argc - 1) {
            /*initialize all the relevant DS for the next file*/
            init();
        }
    }

    return 0;
}
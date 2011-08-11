//
//  helpers.c
//
//
//  Created by Danny Arnold on 2011-07-20.
//  Copyright 2011 TU Bergakademie Freiberg. All rights reserved.
//


#include "helpers.h"
#include "stdio.h"
#include <stdlib.h>

// =============
// = abortprog =
// =============
/*
	Reports the error number and the routine it occurred in before it exits
	Though i don't use it.
*/
void abortprog(int lineno, char sr_name[10], LI error_no) {
    fprintf(stdout,"\nChemApp error no. %li occurred when calling %s.\n"
            "Aborting on line %i of %s .\n", error_no, sr_name,
            lineno, __FILE__);
    exit(error_no);
}

void table() {


    LI i, noerr, nphase;
    DB amount, act;
    char name[TQSTRLEN];


    /* Retrieve and display the activities and equilibrium
       amounts of all phases */


    /* Get number of phases */
    tqnop(&nphase, &noerr);


    /* Print table header */
    printf("%-27s%-16s%-16s\n", "Phase name", "amount/mol", "activity");


    for(i = 1; i <= nphase; i++) {


        /* Get the phase name */
        tqgnp(i, name, &noerr);


        /* Get its equilibrium amount */
        tqgetr("a",  i, 0, &amount, &noerr);


        /*  Get its activity */
        tqgetr("ac",  i, 0, &act, &noerr);


        printf("%-24s%14.5f%14.5f\n", name, amount, act);


    }
}

void table_eliminate(int eliminated[]) {


    LI i, noerr, nphase;
    noerr=0;
    nphase=0;
    DB amount, act;
    char name[TQSTRLEN];


    /* Retrieve and display the activities and equilibrium
       amounts of all phases */


    /* Get number of phases */
    tqnop(&nphase, &noerr);

    for(i = 1; i <= nphase; i++) {


        /* Get the phase name */
        tqgnp(i, name, &noerr);


        /* Get its equilibrium amount */
        tqgetr("a",  i, 0, &amount, &noerr);


        /*  Get its activity */
        tqgetr("ac",  i, 0, &act, &noerr);


        // if ((amount == 0) && (act == 0)) {
        if ((amount != 0)) {
            // tqcsp(i, "eliminated", &noerr);
            eliminated[i-1] = 1;
        }

    }
}

void table_count(DB total_amount[]) {


    LI noerr, nphase;
    DB amount, act;
    char name[TQSTRLEN];


    /* Retrieve and display the activities and equilibrium
       amounts of all phases */


    /* Get number of phases */
    tqnop(&nphase, &noerr);

    for(int i = 1; i <= nphase; i++) {
        /* Get its equilibrium amount */
        tqgetr("a",  i, 0, &amount, &noerr);


        /*  Get its activity */
        tqgetr("ac",  i, 0, &act, &noerr);


        total_amount[i-1] += amount;
    }
}

void table_show(DB total_amount[]) {

    LI noerr, nphase;
    char name[TQSTRLEN];


    /* Retrieve and display the activities and equilibrium
       amounts of all phases */

    printf("%-27s%-16s\n", "Phase name", "total amount");


    /* Get number of phases */
    tqnop(&nphase, &noerr);

    for(int i = 1; i <= nphase; i++) {


        /* Get the phase name */
        tqgnp(i, name, &noerr);


        printf("%-24s%14.5f\n", name, total_amount[i-1]);
    }


}

void table_enter() {


    LI i, noerr, nphase;


    /* Retrieve and display the activities and equilibrium
       amounts of all phases */


    /* Get number of phases */
    tqnop(&nphase, &noerr);

    for(i = 1; i <= nphase; i++) {

        tqcsp(i, "entered", &noerr);

    }

}

void reset_vars(DB *a, DB *b, DB *c, DB *d, DB *e, DB *f) {
    *a = 0;
    *b = 0;
    *c = 0;
    *d = 0;
    *e = 0;
    *f = 0;
}

void set_all(int arr[], int step) {
    LI numcon, noerr, nelements;

		/* Get number of elements */
    tqnosc(&nelements, &noerr);

		for(int i = 1; i <= nelements; i++)
		{
			tqsetc("ia", 0, i, ((DB)arr[i-1]) / step, &numcon, &noerr);
		}
}

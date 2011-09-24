//
//  helpers.c
//
//
//  Created by Danny Arnold on 2011-07-20.
//  Copyright 2011 TU Bergakademie Freiberg. All rights reserved.
//


#include "helpers.h"
#include "stdio.h"
#include "math.h"
#include <stdlib.h>

// =============
// = abortprog =
// =============
/*
	Reports the error number and the routine it occurred in before it exits
	Though i don't use it.
*/
void abortprog(int lineno, char sr_name[10], LI error_no) {
    fprintf(stdout,"\nChemApp error no. %i occurred when calling %s.\n"
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


    for (i = 1; i <= nphase; i++) {


        /* Get the phase name */
        tqgnp(i, name, &noerr);


        /* Get its equilibrium amount */
        tqgetr("a",  i, 0, &amount, &noerr);


        /*  Get its activity */
        tqgetr("ac",  i, 0, &act, &noerr);


        printf("%-24s%14.5f%14.5f\n", name, amount, act);


    }
}

//DB calc_error(int phase)

void eliminate_phases(int eliminate[]) {

    LI noerr, nphases;
    noerr = 0;
    nphases = 0;

    /* Get number of phases */
    tqnop(&nphases, &noerr);

    for (int i = 0; i < nphases; i++) {
        if (eliminate[i] == 0)
        {
            tqcsp(i+1, "eliminated", &noerr);
        }
    }

}


void check_elimination(int eliminated[], double margin) {

    LI i, noerr, nphase;
    noerr=0;
    nphase=0;
    DB amount, act;
    char name[TQSTRLEN];


    /* Retrieve and display the activities and equilibrium
       amounts of all phases */


    /* Get number of phases */
    tqnop(&nphase, &noerr);

    for (i = 1; i <= nphase; i++) {


        /* Get the phase name */
        tqgnp(i, name, &noerr);


        /* Get its equilibrium amount */
        tqgetr("a",  i, 0, &amount, &noerr);


        /*  Get its activity */
        tqgetr("ac",  i, 0, &act, &noerr);


        if ((amount - margin > 0)) {
            eliminated[i-1] = 1;
        }

    }
}

DB calc_error(DB amounts, DB amounts_with_eliminations) {


    DB error;

    /*
        TODO remove, only for debugging
    */
    // show_amounts(amounts);
    // show_amounts(amounts_with_eliminations);
    // show_amounts(biggest_error_so_far);
    // getchar();

    error = fabs((amounts - amounts_with_eliminations)) / amounts;

    /*
        TODO remove, only for debugging
    */
    // printf("|(%f - %f)| / %f = %f\n", amounts, amounts_with_eliminations, amounts, error);
    // getchar();

    return error;
}

// ===============================================
// = Returns 1 if there is an error otherwhise 0 =
// ===============================================
int biggest_error_calc(DB amounts[], DB amounts_with_eliminations[], DB biggest_error_so_far[]) {


    int retval = 0;
    LI noerr, nphase;
    DB error;


    /* Get number of phases */
    tqnop(&nphase, &noerr);

    for (int i = 0; i < nphase; i++) {
        error = calc_error(amounts[i], amounts_with_eliminations[i]);
        
        if(error > 0)
        {
            retval = 1;
        }

        if(error > biggest_error_so_far[i])
        {
            biggest_error_so_far[i] = error;
        }
    }
    
    return retval;
}

void get_amounts(DB amounts[]) {


    LI noerr, nphase;
    DB amount;


    /* Retrieve and display the equilibrium
       amounts of all phases */


    /* Get number of phases */
    tqnop(&nphase, &noerr);

    for (int i = 1; i <= nphase; i++) {
        /* Get its equilibrium amount */
        tqgetr("a",  i, 0, &amount, &noerr);


        amounts[i-1] = amount;
    }
}



void count_amounts(DB total_amount[]) {


    LI noerr, nphase;
    DB amount, act;


    /* Retrieve and display the activities and equilibrium
       amounts of all phases */


    /* Get number of phases */
    tqnop(&nphase, &noerr);

    for (int i = 1; i <= nphase; i++) {
        /* Get its equilibrium amount */
        tqgetr("a",  i, 0, &amount, &noerr);


        /*  Get its activity */
        tqgetr("ac",  i, 0, &act, &noerr);


        total_amount[i-1] += amount;
    }
}

void show_amounts(DB total_amount[]) {

    LI noerr, nphase;
    char name[TQSTRLEN];


    /* Retrieve and display the activities and equilibrium
       amounts of all phases */

    printf("%-27s%-16s\n", "Phase name", "total amount");


    /* Get number of phases */
    tqnop(&nphase, &noerr);

    for (int i = 1; i <= nphase; i++) {


        /* Get the phase name */
        tqgnp(i, name, &noerr);


        printf("%-24s%14.5f\n", name, total_amount[i-1]);
    }


}

void enter_all_phases() {


    LI i, noerr, nphase;


    /* Retrieve and display the activities and equilibrium
       amounts of all phases */


    /* Get number of phases */
    tqnop(&nphase, &noerr);

    for (i = 1; i <= nphase; i++) {

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

void set_all_ia(int arr[], int step) {
    LI numcon, noerr, nelements;

    /* Get number of elements */
    tqnosc(&nelements, &noerr);

    for (int i = 1; i <= nelements; i++)
    {
        if(i!=6)
        {
            tqsetc("ia", 0, i, ((DB)arr[i-1]) / step, &numcon, &noerr);
        }
    }
}



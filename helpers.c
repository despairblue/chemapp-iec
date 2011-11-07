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

// ===========================================================================
// = Reports the error number and the routine it occurred in before it exits =
// ===========================================================================
void abortprog(int lineno, char sr_name[10], LI error_no, char* file) {
    LI noerr;
    char answer;
    
    /* -1 for not asked, 0 for not silent, 1 for silent */
    static int silent = -1;

    /* count total errors */
    show_total_chemapp_errors(0);

    if (silent == -1) {
        fprintf(stdout,"\nChemApp error no. %i occurred when calling %s\non line %i of %s .\n", error_no, sr_name, lineno, file);
        puts("Ignore further errors [yN]?");
        fflush(NULL);
        answer = getchar();

        if (answer == 'y') {
            silent = 1;
        } else {
            silent = 0;
        }
    }

    if (silent == 0) {
        fprintf(stdout,"\nChemApp error no. %i occurred when calling %s\non line %i of %s .\n", error_no, sr_name, lineno, file);
        tqshow(&noerr);

        puts("Exit [yN]?");
        fflush(NULL);
        answer = getchar();
        if (answer == 'y') {
            exit(error_no);
        }
    }
}

// =====================================================================
// = Counts all calls to abortprog                                     =
// = If (show_them == 1) it will print them, otherwise it stays silent =
// =====================================================================
int show_total_chemapp_errors (int show_them) {

    static int count = 0;

    if (show_them) {
        printf("\nTotal chemapp errors: %i\n", count);
    } else {
        count++;
    }

    return count;

}

// ===========================================================
// = Prints a table with all phase name, amount and activity =
// ===========================================================
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

// =================================================================================
// = Takes an array as arguments, each phase which (index == 0) will be eliminated =
// =================================================================================
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

// ==========================================================================================
// = Checks which phases could be eliminated in the current equilibrium                     =
// = margin is the offset from zero which is still thought to be low enough for elimination =
// ==========================================================================================
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

// ===========================================================
// = Returns the error which was causes by elimating a phase =
// ===========================================================
DB calc_error(DB amounts, DB amounts_with_eliminations) {

    DB error;

    error = fabs((amounts - amounts_with_eliminations)) / amounts;

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

// ==================================================
// = Retrieve the equilibrium amounts of all phases =
// ==================================================
void get_amounts(DB amounts[]) {

    LI noerr, nphase;
    DB amount;

    /* Get number of phases */
    tqnop(&nphase, &noerr);

    for (int i = 1; i <= nphase; i++) {
        /* Get its equilibrium amount */
        tqgetr("a",  i, 0, &amount, &noerr);


        amounts[i-1] = amount;
    }
}

// ===================================================================
// = Adds up the current amounts in the equilibrium to total_amounts =
// ===================================================================
void count_amounts(DB total_amount[]) {

    LI noerr, nphase;
    DB amount, act;

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

// =====================================================
// = Prints the total amount for all phases in a table =
// =====================================================
void show_amounts(DB total_amount[]) {

    LI noerr, nphase;
    char name[TQSTRLEN];

    printf("%-27s%-16s\n", "Phase name", "total amount");


    /* Get number of phases */
    tqnop(&nphase, &noerr);

    for (int i = 1; i <= nphase; i++) {


        /* Get the phase name */
        tqgnp(i, name, &noerr);


        printf("%-24s%14.5f\n", name, total_amount[i-1]);
    }


}

// =====================
// = Enters all phases =
// =====================
void enter_all_phases() {

    LI i, noerr, nphase;

    /* Get number of phases */
    tqnop(&nphase, &noerr);

    for (i = 1; i <= nphase; i++) {

        tqcsp(i, "entered", &noerr);

    }

}

// ========================
// = Resets all vars to 0 =
// ========================
void reset_vars(DB *a, DB *b, DB *c, DB *d, DB *e, DB *f) {
    *a = 0;
    *b = 0;
    *c = 0;
    *d = 0;
    *e = 0;
    *f = 0;
}

// ==========================================
// = Sets the initial amount for all phases =
// ==========================================
void set_all_ia(int arr[], int step, int* ignored_elements) {
    LI numcon, noerr, nelements;

    /* Get number of elements */
    tqnosc(&nelements, &noerr);

    for (int i = 1; i <= nelements; i++)
    {
        if( check_for_ignored_element(i-1, ignored_elements) == 0 ) {
            tqsetc("ia", 0, i, ((DB)arr[i-1]) / step, &numcon, &noerr);
            if (noerr) abortprog(__LINE__,"tqsetc",noerr, __FILE__);
        }
    }
}

// ==============================================
// = Returns 1 if the element should be ignored =
// ==============================================
int check_for_ignored_element(int element, int* ignored_elements) {
    if ( ((long int)ignored_elements) != 0 ) {
        if (ignored_elements[element] == 0) {
            return 1;
        }
    }

    return 0;

}

// ========================================
// = Sums up an array and returns the sum =
// ========================================
int sum_array(int arr[], int n) {
    int sum = 0;

    for (int i = 0; i < n; ++i)
    {
        sum += arr[i];
    }

    return sum;
}

// ================================================
// = Returns 0 if out any element is out of range =
// ================================================
int check_for_range(int arr[], int min_set_ranges[], int max_set_ranges[], int nelements) {
    for (int i = 0; i < nelements; i++) {
        if ( (arr[i] < min_set_ranges[i]) || (arr[i] > max_set_ranges[i]) ) {
            return 0;
        };
    }

    return 1;
}

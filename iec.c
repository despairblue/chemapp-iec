//
//  iec.c
//  «faster, harder, chemapper»
//
//  Created by Danny Arnold on 2011-05-01.
//  Copyright 2011 TU Bergakademie Freiberg. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lib64/cacint.h"
#include "iter.h"
#include "helpers.h"

int main ()
{
    /*
    	Variable Declarations
    */

    LI   nelements,               // all-purpose integer variable
     noerr,              // error code return variable
     nphases,             // number of phases in the loaded data-file
     numcon,             // index of a condition set
     unitno;             // FORTRAN unit number of the data-files

    int error_code;

    char *dstrptr;           // pointer to a string

    /*
    	Initializing and Stuff
    */

    // printf("\nProgram for accelerating the equilibrium calculations\n\n");

    dstrptr = malloc(TQSTRLEN);

    // Initialise ChemApp
    tqini(&noerr);

    // Getting the FORTRAN unit for the data-file
    tqgio("FILE", &unitno, &noerr);

    // Open data-file for reading
    tqopna("data/pitzer.dat", unitno, &noerr);
    // tqopna("femgsio4.dat", unitno, &noerr);
    // tqopna("data/cosi.dat", unitno, &noerr);
    // tqopnt("data/Test.cst",10,&noerr);

    // Read data-file
    tqrfil(&noerr);
    if (noerr) abortprog(__LINE__,"tqfil",noerr, __FILE__);

    // Read cst-file
    // tqrcst(&noerr);
    // if (noerr) abortprog(__LINE__,"tqrcst",noerr, __FILE__);

    // Close file
    tqclos(unitno, &noerr);
    if (noerr) abortprog(__LINE__,"tqclos",noerr, __FILE__);

    // get number of phases
    tqnop(&nphases, &noerr);
    printf("Number of phases: %i\n", nphases);
    tqnosc(&nelements,&noerr);
    printf("Number of elements: %i\n", nelements);
    tqcsu("Amount", "gram", &noerr);
    //tqcsu("Amount", "mol", &noerr);
    tqshow(&noerr);

    // input struct for the iteration
    struct iteration_input id = ITERATION_INPUT_DEFAULT;
    id.t_min = 1000;
    id.t_max = 1010;
    id.p_min = 1;
    id.p_max = 10;

    // id.margin = 0.999;

    id.do_set_ranges = 1;
    int min_set_ran[] = {5, 6, 5, 5, 5, 0};
    int max_set_ran[] = {5, 5, 5, 5, 5, 10};
    id.min_set_ranges = min_set_ran;
    id.max_set_ranges = max_set_ran;

    // id.do_ignore_elements = 1;
    int test[] = {1,1,1,1,1,0};
    // int test[] = {1,1,0};
    id.ignored_elements = test;
    // id.do_eliminate = 1;
    // id.eliminate = test;
    // id.do_calc_errors = 1;

    // output struct for the iteration
    struct iteration_output od = ITERATION_OUTPUT_DEFAULT;

    // start iteration with all components
    puts("******************************************");
    puts("* Start calculation with all components. *");
    puts("******************************************\n");

    // print settings
    print_settings(id, nelements, nphases);

    // run iteration with the parameters set in id
    error_code = run_iteration(id, &od);
    if(error_code != 0)
    {
        printf("%s\n", error_code_to_str(error_code));
        exit(error_code);
    }

    printf("\nTime: %i\n___________________________\n\n", od.time_taken);

    puts("Phases that that can't be eliminated are marked with a 1:");
    for (int i = 0; i < nphases; ++i)
    {
        printf("%d: %d\n", i+1, od.eliminated_phases[i]);
    }
    show_total_chemapp_errors(1);
    printf("\n\n\n");

    // TODO: debugging stuff
    exit(0);

    // changing settings

    // setting temperature and pressure
    tqsetc("T", 0, 0, 1000, &numcon, &noerr);
    tqsetc("P", 0, 0, 1, &numcon, &noerr);

    // id.do_tqshow = 1;
    id.eliminated_phases = od.eliminated_phases;
    id.do_eliminate = 1;
    id.do_calc_errors = 0;

    // start iteration without some phases
    puts("********************************************");
    puts("   Start calculation without some Phases.");
    puts("********************************************\n");

    // print settings
    print_settings(id, nelements, nphases);

    error_code = run_iteration(id, &od);
    if(error_code != 0)
    {
        printf("%s\n", error_code_to_str(error_code));
    }

    // table();
    printf("\nTime: %i\n___________________________\n\n", od.time_taken);

    return 0;
}

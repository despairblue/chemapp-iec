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
#include "cacint.h"
#include "iter.h"
#include "helpers.h"

int main ()
{
    /*
    	Variable Declarations
    */

    LI   lint,               // all-purpose integer variable
         noerr,              // error code return variable
         nphase,             // number of phases in the loaded data-file
         numcon,             // index of a condition set
         unitno;             // FORTRAN unit number of the data-files

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
    // tqopna("cosi.dat", unitno, &noerr);
    //tqopna("femgsio4.dat", unitno, &noerr);
    tqopna("cosi.dat", unitno, &noerr);
    //tqopnt("Test.cst",10,&noerr);

    // Read data-file
    tqrfil(&noerr);
    //tqrcst(&noerr);
    // Close data-file
    tqclos(unitno, &noerr);

    // get number of phases
    tqnop(&nphase, &noerr);
    printf("Number of phases: %li\n", nphase);
    tqnosc(&lint,&noerr);
    printf("Number of elements: %li\n", lint);
    tqcsu("Amount", "gram", &noerr);
    //tqcsu("Amount", "mol", &noerr);
    tqshow(&noerr);

    // input struct for the iteration
    struct iteration_input id;
    id.t_min = 1000;
    id.t_max = 1010;
    id.p_min = 1;
    id.p_max = 10;

		id.step = 10;

    id.do_tqshow = 0;
    id.do_tqcenl = 0;
    id.do_table = 0;
    id.do_eliminate = 0;
		id.do_test = 0;

		id.eliminate = 0;

    // output struct for the iteration
    struct iteration_output od;
    od.time_taken = 0;

    // start iteration with all components
    puts("********************************************");
    puts("   Start calculation with all components.");
    puts("********************************************\n");

    // run iteration with the parameters set in id
    run_iteration(id, &od);

    // table();
    printf("\nTime: %li\n___________________________\n\n", od.time_taken);

		puts("\nPhases that could be eliminated:");
    for (int i = 0; i < nphase; ++i)
    {
        printf("%d: %d\n", i+1, od.eliminated[i]);
    }
		printf("\n\n\n");

    // start iteration without come phases
    puts("********************************************");
    puts("   Start calculation without some Phases.");
    puts("********************************************\n");

    // setting temperature and pressure
    tqsetc("T", 0, 0, 1000, &numcon, &noerr);
    tqsetc("P", 0, 0, 1, &numcon, &noerr);

		// id.do_tqshow = 1;
		id.eliminate = od.eliminated;
    id.do_eliminate = 1;
    run_iteration(id, &od);

    // table();
    printf("\nTime: %li\n___________________________\n\n", od.time_taken);

    return 0;
}

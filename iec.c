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

int main (int argc, char const *argv[])
{
	/*
		Variable Declarations
	*/
	
	LI   lint,               // all-purpose integer variable
	     time_taken, time_total,
	     indexp,             // number of phases
	     indexc,             // phase constituents
	     indexl,             // sublattices
	     noerr,              // error code return variable
	     nphase,             // number of phases in the loaded data-file
	     npcgas,             // number of phase constituents in the gas phase
	     numcon,             // index of a condition set
	     unitno,             // FORTRAN unit number of the data-files
	     errorunit;          // FORTRAN unit number for error messages

	DB   db1,                 // all-purpose real variable
	     darray2[2],         // all-purpose 2 var array
	     darray30[30],       // all-purpose 30 var array
	     c_all, c_mean,
	     o_all, o_mean,
	     si_all, si_mean,
	     *darray,            // dynamic array
	     tp[2];              // temperature and pressure of a stream

	char dstr[TQSTRLEN],     // all-purpose static string
	     dstr2[TQSTRLEN],    // all-purpose static string
	     *dstrptr,           // pointer to a string
	     pname[TQSTRLEN],    // names of phases
	     cname[TQSTRLEN],     // names of constituents
	     mname[TQSTRLEN],     // mixture model names
	     newsc[4][TQSTRLEN];  // 4 string array for tqcsc

	int i, j, k;              // all-purpose variables
	double n;                 // all-purpose variables
	
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
	tqopna("cosi.dat", unitno, &noerr);
	
	// Read data-file
	tqrfil(&noerr);
	
	// Close data-file
	tqclos(unitno, &noerr);
	
	// get number of phases
	tqnop(&nphase, &noerr);
	printf("Number of phases: %li\n", nphase);
	
	tqshow(&noerr);
	
	// initialize vars
	reset_vars(&c_all, &c_mean, &o_all, &o_mean, &si_all, &si_mean);
	
	// start iteration with all components
	puts("********************************************");
	puts("   Start calculation with all components.");
	puts("********************************************\n");
	
	// struct for the iteration
	struct iteration_data id;
	id.t_min = 1000;
	id.t_max = 1010;
	id.p_min = 1;
	id.p_max = 20;
	id.time_taken = &time_taken;
	id.numcon = &numcon;
	id.darray2 = darray2;
	id.noerr = &noerr;
	id.do_tqshow = 0;
	id.do_tqcenl = 0;
	id.do_table = 0;
	id.do_eliminate = 0; 
	
	time_total = 0;
	
	// run iteration with the parameters set in id
	run_iteration(id);
	
	table();
	printf("Time: %li\n___________________________\n\n", *id.time_taken);

	// start iteration without Si
	puts("********************************************");
	puts("   Start calculation without some Phases.");
	puts("********************************************\n");
	
	// setting temperature and pressure
	tqsetc("T", 0, 0, 1000, &numcon, &noerr);
	tqsetc("P", 0, 0, 1, &numcon, &noerr);
	
	for(i = 4; i < 9; ++i)
	{
		tqcsp(i, "eliminated", &noerr);
	}
	
	time_total = 0;
	
	// id.do_tqshow = 1;
	id.do_eliminate = 1;
	run_iteration(id);
	time_total += *id.time_taken;
	
	table();
	printf("Time: %li\n___________________________\n\n", time_total);
	
	return 0;
}

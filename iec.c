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
#include "cacint.h"


// =============
// = abortprog =
// =============
/*
	Reports the error number and the routine it occurred in before it exits
*/
void abortprog(int lineno, char sr_name[10], LI error_no)
{
	fprintf(stdout,"\nChemApp error no. %li occurred when calling %s.\n"
		"Aborting on line %i of %s .\n", error_no, sr_name, 
		lineno, __FILE__);
	exit(error_no);
}

int main (int argc, char const *argv[])
{
	/*
		Variable Declarations
	*/
	
	LI   lint,               // all-purpose integer variable
	     indexp,             // number of phases
	     indexc,             // phase constituents
	     indexl,             // sublattices
	     noerr,              // error code return variable
	     nphase,             // number of phases in the loaded data-file
	     npcgas,             // number of phase constituents in the gas phase
	     unitno,             // FORTRAN unit number of the data-files
	     errorunit;          // FORTRAN unit number for error messages

	DB   db1,                // all-purpose integer variable
	     darray2[2],         // all-purpose 2 var array
	     darray30[30],       // all-purpose 30 var array
	     *darray,            // dynamic array
	     tp[2];              // temperature and pressure of a stream

	char dstr[TQSTRLEN],     // all-purpose static string
	     dstr2[TQSTRLEN],    // all-purpose static string
	     *dstrptr,           // pointer to a string
	     pname[TQSTRLEN],    // names of phases
	     cname[TQSTRLEN],     // names of constituents
	     mname[TQSTRLEN],     // mixture model names
	     newsc[4][TQSTRLEN];  // 4 string array for tqcsc

	int i;                    // all-purpose variable
	
	/*
		Initializing and Stuff
	*/
	
	printf("\nProgram for accelerating the equilibrium calculations\n\n");

	dstrptr = malloc(TQSTRLEN);
	
	// Initialise ChemApp
	tqini(&noerr);
	
	return 0;
}
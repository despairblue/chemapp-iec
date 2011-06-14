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
	     numcon,             // index of a condition set
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

	int i, j;                 // all-purpose variables
	
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
	// printf("Number of phases: %li\n", nphase);
	
	// Set equilibrium conditions
	tqsetc("ia ", 1, 4, 1.0, &numcon, &noerr);
	for(i = 1; i <= 100; ++i)
	{
		tqsetc("T", 0, 0, i, &numcon, &noerr);
		
		for(j = 1; j <= 200; ++j)
		{
			tqsetc("P", 0, 0, j, &numcon, &noerr);
			// Display present settings
			// printf("\n\nCurrently active conditions:\n");
			// printf("\n\n**** Begin output table produced by tqshow\n");
			// fflush(NULL);
			// tqshow(&noerr);
			// fflush(NULL);
			// printf("\n**** End output table produced by tqshow\n\n\n");

			// Calcualte equilibrium
			darray2[0] = 0.0;
			tqce(" ", 0, 0, darray2, &noerr);

			// Print ChemSage output table
			// printf("\n\n**** Begin output table produced by tqcenl\n");
			// 		  fflush(NULL);
			// 		  tqcenl(" ",0,0,darray2,&noerr); 
			// 		  fflush(NULL);
			// 		  printf("\n**** End output table produced by tqcenl\n\n\n");
		}
	}
	
	return 0;
}
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

// ==================
// = iteration_data =
// ==================
/*
	Takes all arguments needed for the iteration
*/
struct iteration_data {
	int t_min;
	int t_max;
	int p_min;
	int p_max;
	LI* time_taken;
	LI* numcon;
	DB* darray2;
	LI* noerr;
	int do_tqshow;
};

// =============
// = abortprog =
// =============
/*
	Reports the error number and the routine it occurred in before it exits
	Though i don't use it.
*/
void abortprog(int lineno, char sr_name[10], LI error_no)
{
	fprintf(stdout,"\nChemApp error no. %li occurred when calling %s.\n"
		"Aborting on line %i of %s .\n", error_no, sr_name, 
		lineno, __FILE__);
	exit(error_no);
}

void reset_vars(DB *a, DB *b, DB *c, DB *d, DB *e, DB *f)
{
	*a = 0;
	*b = 0;
	*c = 0;
	*d = 0;
	*e = 0;
	*f = 0;
}

// void run_iteration(int t_min, int t_max, int p_min, int p_max, LI *time_taken, LI *numcon, DB darray2[2], LI *noerr)
void run_iteration(struct iteration_data id)
{
	int t_min = id.t_min;
	int t_max = id.t_max;
	int p_min = id.p_min;
	int p_max = id.p_max;
	LI *time_taken = id.time_taken;
	LI *numcon = id.numcon;
	DB *darray2 = id.darray2;
	LI *noerr = id.noerr;
	
	int now, then, i, j;
	DB db1, c_all, c_mean, o_all, o_mean, si_all, si_mean;
	char dstr2[TQSTRLEN];
	
	reset_vars(&c_all, &c_mean, &o_all, &o_mean, &si_all, &si_mean);
	
	now = time(NULL);
	for(i = t_min; i <= t_max; ++i)
	{
		tqsetc("T", 0, 0, i, numcon, noerr);
		
		for(j = p_min; j <= p_max; ++j)
		{
			tqsetc("P", 0, 0, j, numcon, noerr);
			// Display present settings if do_tqshow == 1
			if (id.do_tqshow == 1) {
				printf("\n\n**** Begin output table produced by tqshow\n");
				fflush(NULL);
				tqshow(noerr);
				fflush(NULL);
				printf("\n**** End output table produced by tqshow\n\n\n");
				// wait for enter to continue
				getchar();
			}

			// Calcualte equilibrium
			darray2[0] = 0.0;
			tqcen(" ", 0, 0, darray2, noerr);
			tqgnsc(1, dstr2, noerr);
			tqgetr("XP", 1, 1, &db1, noerr);
			// printf("Mole fraction of %s in the GAS phase: %f\n\n", dstr2, db1);
			c_all += db1;
			c_mean += 1;
			
			tqgetr("XP", 2, 1, &db1, noerr);
			o_all += db1;
			o_mean += 1;
			
			tqgetr("XP", 3, 1, &db1, noerr);
			si_all += db1;
			si_mean += 1;

			// Print ChemSage output table
			// printf("\n\n**** Begin output table produced by tqcenl\n");
			// 		  fflush(NULL);
			// 		  tqcenl(" ",0,0,darray2,&noerr); 
			// 		  fflush(NULL);
			// 		  printf("\n**** End output table produced by tqcenl\n\n\n");
		}
	}
	then = time(NULL);
	
	printf("Mean mole fraction of C in the GAS phase %f\n", c_all / c_mean);
	printf("Existence of C: %f\n\n", c_all);
	printf("Mean mole fraction of O in the GAS phase %f\n", o_all / o_mean);
	printf("Existence of O: %f\n\n", o_all);
	printf("Mean mole fraction of Si in the GAS phase %f\n", si_all / si_mean);
	printf("Existence of Si: %f\n\n", si_all);
	
	*time_taken = then - now;
}

int main (int argc, char const *argv[])
{
	/*
		Variable Declarations
	*/
	
	LI   lint,               // all-purpose integer variable
	     now, then,
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
	printf("Number of phases: %li\n", nphase);
	
	// Set equilibrium conditions
	tqsetc("ia", 1, 1, 1.0, &numcon, &noerr);
	tqsetc("ia", 2, 1, 0.9, &numcon, &noerr);
	tqsetc("ia", 3, 1, 0.5, &numcon, &noerr);
	
	// initialize vars
	reset_vars(&c_all, &c_mean, &o_all, &o_mean, &si_all, &si_mean);
	
	// start iteration with all components
	puts("********************************************");
	puts("   Start calculation with all components.");
	puts("********************************************\n");
	
	tqce(" ", 0, 0, darray2, &noerr);
	
	tqgnsc(1, dstr2, &noerr);
	tqgetr("XP", 1, 1, &db1, &noerr);
	printf("Mole fraction of %s in the GAS phase: %f\n\n", dstr2, db1);
	
	tqgnsc(2, dstr2, &noerr);
	tqgetr("XP", 2, 1, &db1, &noerr);
	printf("Mole fraction of %s in the GAS phase: %f\n\n", dstr2, db1);
	
	tqgnsc(3, dstr2, &noerr);
	tqgetr("XP", 3, 1, &db1, &noerr);
	printf("Mole fraction of %s in the GAS phase: %f\n\n", dstr2, db1);
	
	tqce(" ", 0, 0, darray2, &noerr);
	
	// struct for the iteration
	struct iteration_data id;
	id.t_min = 1000;
	id.t_max = 1100;
	id.p_min = 1;
	id.p_max = 20;
	id.time_taken = &now;
	id.numcon = &numcon;
	id.darray2 = darray2;
	id.noerr = &noerr;
	id.do_tqshow = 0;
	
	// run iteration with the parameters set in id
	run_iteration(id);
	printf("Time: %li\n___________________________\n\n", now);

	// start iteration without Si
	puts("********************************************");
	puts("   Start calculation without some Phases.");
	puts("********************************************\n");
	
	// setting temperature and pressure
	tqsetc("T", 0, 0, 1000, &numcon, &noerr);
	tqsetc("P", 0, 0, 1, &numcon, &noerr);
	
	// for(i = 1; i < 9; ++i)
	// {
	// 	tqcsp(i, "eliminated", &noerr);
	// }
	
	// eliminate C
	tqcsp(2, "eliminated", &noerr);
	
	tqce(" ", 0, 0, darray2, &noerr);
	
	// id.do_tqshow = 1;
	run_iteration(id);
	printf("Time: %li\n___________________________\n\n", now);
	
	return 0;
}
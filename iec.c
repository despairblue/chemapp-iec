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
	int do_tqcenl;
	int do_table;
	int do_eliminate;
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

void table()
{


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

void table_eliminate()
{


  LI i, noerr, nphase;
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
		if ((amount == 0)) {
			tqcsp(i, "eliminated", &noerr);
		}
		
  }
}

void table_enter()
{


  LI i, noerr, nphase;


  /* Retrieve and display the activities and equilibrium 
     amounts of all phases */


  /* Get number of phases */
  tqnop(&nphase, &noerr);

  for(i = 1; i <= nphase; i++) {
	
			tqcsp(i, "entered", &noerr);
			
	}
		
}

// TODO: table_count: adds all amount of elements of the current equilibrium to a total amount count
// TODO: table_show: shows the totals

void reset_vars(DB *a, DB *b, DB *c, DB *d, DB *e, DB *f)
{
	*a = 0;
	*b = 0;
	*c = 0;
	*d = 0;
	*e = 0;
	*f = 0;
}

void set_all(double a, double b, double c) {
	LI numcon, noerr;
	
	tqsetc("ia", 0 , 1, a, &numcon, &noerr);
	tqsetc("ia", 0 , 2, a, &numcon, &noerr);
	tqsetc("ia", 0 , 3, a, &numcon, &noerr);
}

// void run_iteration(int t_min, int t_max, int p_min, int p_max, LI *time_taken, LI *numcon, DB darray2[2], LI *noerr)
void run_iteration(struct iteration_data id)
{
	int t_min = id.t_min;
	int t_max = id.t_max;
	int p_min = id.p_min;
	int p_max = id.p_max;
	LI *numcon = id.numcon;
	DB *darray2 = id.darray2;
	LI *noerr = id.noerr;
	
	int now, then, i, j, k, l;
	double n, m, o;
	DB db1, c_all, c_mean, o_all, o_mean, si_all, si_mean;
	char dstr2[TQSTRLEN];
	LI nphase;
	
	tqnop(&nphase, noerr);
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
			
			if(id.do_eliminate == 1)
			{
				table_enter();
			}
			for(n = 0.1; n < 0.5; n += 0.1)
			{
				for(m = 0.1; m < 0.5; m += 0.1)
				{
					for(o = 0.1; o < 0.5; o += 0.1)
					{
						// tqsetc("ia", 0 , 1 2 3, n, numcon, noerr);
						// if (noerr) abortprog(__LINE__,"tqsetc", *noerr);
						set_all(n, m, o);
						darray2[0] = 0.0;
						tqce(" ", 0, 0, darray2, noerr);
						if (id.do_eliminate == 1) {
							table_eliminate();
						}
						// if (noerr) abortprog(__LINE__,"tqce", *noerr);
					}

				}
			}

			// Calcualte equilibrium
			tqgetr("a", 1, 1, &db1, noerr);
			c_all += db1;
			c_mean += 1;
			
			tqgetr("a", 2, 1, &db1, noerr);
			o_all += db1;
			o_mean += 1;
			
			tqgetr("a", 3, 1, &db1, noerr);
			si_all += db1;
			si_mean += 1;
		}
	}
	then = time(NULL);
	
	// Print ChemSage output table
	if (id.do_tqcenl == 1) {
		printf("\n\n**** Begin output table produced by tqcenl\n");
		fflush(NULL);
		tqcenl(" ", 0, 0, darray2, noerr); 
		fflush(NULL);
		printf("\n**** End output table produced by tqcenl\n\n\n");
	}
	
	if(id.do_table == 1)
	{
		table();
	}
	
	*id.time_taken = then - now;
}

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

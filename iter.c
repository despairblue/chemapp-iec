#include "iter.h"
#include "helpers.h"
#include "stdlib.h"
#include "time.h"
#include "stdio.h"

void run_iteration(struct iteration_input id, struct iteration_output* od) {

	int t_min = id.t_min;
	int t_max = id.t_max;
	int p_min = id.p_min;
	int p_max = id.p_max;
	LI numcon;
	DB darray2[2];
	LI noerr;
	
	int now, then, i, j, k, l;
	double n, m, o;
	DB db1, c_all, c_mean, o_all, o_mean, si_all, si_mean;
	char dstr2[TQSTRLEN];
	LI nphases, nelements;
	
	// number of elements
	tqnosc(&nelements,&noerr);
	// number of phases
	tqnop(&nphases, &noerr);
	
	// will be 1 for all phases not having any amount
	int *eliminated = malloc(nphases * sizeof(int));
	
	// will contain the total amount of any phase
	DB  *total_amount = malloc(nphases * sizeof(DB));
	
	// initializing arrays to zero
	for(i = 0; i < nphases; ++i) { eliminated[i] = 0 ; total_amount[i] = 0; }
	
	reset_vars(&c_all, &c_mean, &o_all, &o_mean, &si_all, &si_mean);
	
	if (id.do_eliminate == 1) {
		tqcsp(1, "eliminated", &noerr);
		// tqcsp(4, "eliminated", noerr);
		tqcsp(3, "eliminated", &noerr);
		tqcsp(6, "eliminated", &noerr);
		tqcsp(7, "eliminated", &noerr);
		tqcsp(8, "eliminated", &noerr);
	}
	
	// current time before iteration
	now = time(NULL);
	
	for(i = t_min; i <= t_max; ++i)
	{
		tqsetc("T", 0, 0, i, &numcon, &noerr);
		
		for(j = p_min; j <= p_max; ++j)
		{
			tqsetc("P", 0, 0, j, &numcon, &noerr);
			
			// Display present settings if do_tqshow == 1
			if (id.do_tqshow == 1) {
				printf("\n\n**** Begin output table produced by tqshow\n");
				fflush(NULL);
				tqshow(&noerr);
				fflush(NULL);
				printf("\n**** End output table produced by tqshow\n\n\n");
				// wait for enter to continue
				getchar();
			}
			
			for(n = 1.0; n >= 0; n -= 0.1)
			{
				for(m = 1.0; m >= 0; m -= 0.1)
				{
					for(o = 1.0; o >= 0; o -= 0.1)
					{
						// tqsetc("ia", 0 , 1 2 3, n, numcon, noerr);
						// if (noerr) abortprog(__LINE__,"tqsetc", *noerr);
						if ((n+m+o > 0.95) && (n+m+o < 1.05)) {
						// if ((n+m+o) == 1.0) {
							// printf("%f + %f + %f = %f\n", n, m, o, n+m+o);
							// getchar();
							set_all(n, m, o);
							darray2[0] = 0.0;
							tqce(" ", 0, 0, darray2, &noerr);
							table_count(total_amount);
							table_eliminate(eliminated);
						}
						// if (noerr) abortprog(__LINE__,"tqce", *noerr);
					}

				}
			}

			// Calcualte equilibrium
			tqgetr("a", 1, 1, &db1, &noerr);
			c_all += db1;
			c_mean += 1;
			
			tqgetr("a", 2, 1, &db1, &noerr);
			o_all += db1;
			o_mean += 1;
			
			tqgetr("a", 3, 1, &db1, &noerr);
			si_all += db1;
			si_mean += 1;
		}
	}
	
	// current time after iteration
	then = time(NULL);
	
	// Print ChemSage output table
	if (id.do_tqcenl == 1) {
		printf("\n\n**** Begin output table produced by tqcenl\n");
		fflush(NULL);
		tqcenl(" ", 0, 0, darray2, &noerr); 
		fflush(NULL);
		printf("\n**** End output table produced by tqcenl\n\n\n");
	}
	
	if(id.do_table == 1)
	{
		table();
	}
	
	(*od).time_taken = then - now;
	
	table_show(total_amount);
	if (!id.do_eliminate) {
		puts("\n Phases that could be eliminated:");
		for(i = 0; i < 8; ++i)
		{
			printf("%d: %d\n", i+1, eliminated[i]);
		}
	}
}
//
//  iter.c
//
//
//  Created by Danny Arnold on 2011-07-20.
//  Copyright 2011 TU Bergakademie Freiberg. All rights reserved.
//


#include "iter.h"
#include "helpers.h"
#include "stdlib.h"
#include "time.h"
#include "stdio.h"

#include "math.h"

void next(int arr[], int size, int lower_bound, int upper_bound) {
	int pointer = size - 1;
	
	if(pointer < 0)
	{
		arr[0]++;
	}
	if ((arr[ pointer ] >= upper_bound) && (pointer > 0)) {
		arr[pointer] = lower_bound;
		next(arr, pointer, lower_bound, upper_bound);
	} else {
		arr[pointer]++;
	}
}

void run_iteration(struct iteration_input id, struct iteration_output* od) {

    int t_min = id.t_min;
    int t_max = id.t_max;
    int p_min = id.p_min;
    int p_max = id.p_max;
		int step  = id.step;
    LI numcon;
    DB darray2[2];
    LI noerr;
		int* eliminate = id.eliminate;

    int now, then, count_all, count_done, sum;
    LI nphases, nelements;

    // number of elements
    tqnosc(&nelements,&noerr);
    // number of phases
    tqnop(&nphases, &noerr);

    // will be 1 for all phases not having any amount
    int *eliminated = malloc(nphases * sizeof(int));
		int *loop = malloc(nelements * sizeof(int));

    // will contain the total amount of any phase
    DB *total_amount = malloc(nphases * sizeof(DB));

    // initializing arrays to zero
    for (int i = 0; i < nphases; ++i) {
        eliminated[i] = 0 ;
        total_amount[i] = 0;
    }

    if (id.do_eliminate == 1) {
        for(LI i = 0; i < nphases; ++i)
        {
					if(eliminate[i] == 0)
					{
						tqcsp(i+1, "eliminated", &noerr);
					}
        }
    }

    // current time before iteration
    now = time(NULL);

    for (int i = t_min; i <= t_max; ++i)
    {
        tqsetc("T", 0, 0, i, &numcon, &noerr);

        for (int j = p_min; j <= p_max; ++j)
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

            count_done = 0;
            count_all = 0;

						for(int i = 0; i < nelements; ++i)
						{
							loop[i] = 0;
						}

						loop[nelements - 1] = 1;
						
						while (loop[0] <= step) {
							
							sum = 0;
							for(LI i = 0; i < nelements; ++i)
							{
								sum += loop[i];
							}
							
							if (sum == step) {
                  set_all(loop, step);

                  darray2[0] = 0.0;
                  tqce(" ", 0, 0, darray2, &noerr);
                  table_count(total_amount);

                  table_eliminate(eliminated);
              }
							
							next(loop, nelements, 0, step);
						}
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

    if (id.do_table == 1)
    {
        table();
    }

    (*od).time_taken = then - now;
		(*od).eliminated = eliminated;

    table_show(total_amount);
}

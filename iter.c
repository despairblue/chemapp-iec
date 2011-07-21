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

void run_iteration(struct iteration_input id, struct iteration_output* od) {

    int t_min = id.t_min;
    int t_max = id.t_max;
    int p_min = id.p_min;
    int p_max = id.p_max;
    LI numcon;
    DB darray2[2];
    LI noerr;

    int now, then, count_all, count_done;
    LI nphases, nelements;

    // number of elements
    tqnosc(&nelements,&noerr);
    // number of phases
    tqnop(&nphases, &noerr);

    // will be 1 for all phases not having any amount
    int *eliminated = malloc(nphases * sizeof(int));

    // will contain the total amount of any phase
    DB *total_amount = malloc(nphases * sizeof(DB));

    int** x;

    x = (int**) malloc(pow(10, nelements) * sizeof(int*));
    for ( int i = 0; i <(int) pow(10, nelements); i++) {
    //    x[i] = (int*) malloc(3 * sizeof(DB));
        for ( int j = 0; j < 3; j++ ) {
    //        x[i][j] = 0.0;
//    	      printf("test %f, %i, %i, %i\n",pow(10.0,3),nelements,i,j);
        }
    }

    // initializing arrays to zero
    for (int i = 0; i < nphases; ++i) {
        eliminated[i] = 0 ;
        total_amount[i] = 0;
    }

    // TODO remove
    // reset_vars(&c_all, &c_mean, &o_all, &o_mean, &si_all, &si_mean);

    if (id.do_eliminate == 1) {
        tqcsp(3, "eliminated", &noerr);
        tqcsp(6, "eliminated", &noerr);
        tqcsp(7, "eliminated", &noerr);
        tqcsp(8, "eliminated", &noerr);
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

            for (double n = 0; n < 1.1; n += 0.1)
            {
                for (double m = 0; m < 1.1; m += 0.1)
                {
                    for (double o = 0; o < 1.1; o += 0.1)
                    {
                        // TODO implement contains() and add()
                        // if ( !contains(x, n, m, o) ) {
                        // add(x, n, m, o);

                        // tqsetc("ia", 0 , 1 2 3, n, numcon, noerr);
                        // if (noerr) abortprog(__LINE__,"tqsetc", *noerr);
                        if ((n+m+o > 0.95) && (n+m+o < 1.05)) {
                            set_all(n, m, o);
                            darray2[0] = 0.0;
                            tqce(" ", 0, 0, darray2, &noerr);
                            table_count(total_amount);
//							table();
//							table_show(total_amount);
//							getchar();
                            table_eliminate(eliminated);

                            // TODO remove
                            count_done++;
  //                          printf("n: %f	m: %f	o: %f\n", n,m,o);
                        }
                        // }

                        // TODO remove
                        count_all++;

                    }
                }
            }

            // TODO remove
            // printf("count_all: %i\ncount_done: %i\n", count_all, count_done );
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

    table_show(total_amount);
    if (!id.do_eliminate) {
        puts("\nPhases that could be eliminated:");
        for (int i = 0; i < 8; ++i)
        {
            printf("%d: %d\n", i+1, eliminated[i]);
        }
    }
}

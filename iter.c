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

    if (pointer < 0)
    {
        arr[0]++;
    }
    if (pointer==5)
    {
        pointer=pointer-1;
    }
    if ((arr[ pointer ] >= upper_bound) && (pointer > 0)) {
        arr[pointer] = lower_bound;
        next(arr, pointer, lower_bound, upper_bound);
    } else {
        arr[pointer]++;
    }
}

// ============================
// = May return an error code =
// ============================
int run_iteration(struct iteration_input id, struct iteration_output* od) {

    int t_min = id.t_min;
    int t_max = id.t_max;
    int p_min = id.p_min;
    int p_max = id.p_max;
    int step  = id.step;
    double margin = id.margin;
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
    
    // containing the initial amounts of the elements
    int *loop = malloc(nelements * sizeof(int));

    // will contain the total amount of any phase
    DB *total_amount = malloc(nphases * sizeof(DB));
    
    // will contain the amounts for that state
    DB *amounts = malloc(nphases * sizeof(DB));
    
    // will contain the amounts for that state with the eliminated phases
    DB *amounts_with_eliminations = malloc(nphases * sizeof(DB));
    
    // will contain the total amount of any phase for the equilibrium with eliminated phases
    DB *total_amount_with_eliminations = malloc(nphases * sizeof(DB));
    
    // will contain the biggest error a phase had
    DB *biggest_error = malloc(nphases * sizeof(DB));

    // initializing arrays to zero
    for (int i = 0; i < nphases; ++i) {
        eliminated[i] = 0 ;
        total_amount[i] = 0;
        amounts[i] = 0;
        total_amount_with_eliminations[i] = 0;
        amounts_with_eliminations[i] = 0;
        biggest_error[i] = 0;
    }

    if (id.do_eliminate == 1) {
        eliminate_phases(eliminate);
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

            for (int i = 0; i < nelements; ++i)
            {
                loop[i] = 0;
            }

            while (loop[0] <= step) {

                sum = 0;
                
                for (LI i = 0; i < nelements; ++i)
                {
                    sum += loop[i];
                }

                if (sum == step) {
                    // set initial amounts for all phases
                    set_all_ia(loop, step);
                    
                    if (id.do_calc_errors == 0)
                    {
                        darray2[0] = 0.0;
                        tqce(" ", 0, 0, darray2, &noerr);

                        count_amounts(total_amount);

                        if (id.do_eliminate == 0) {
                            check_elimination(eliminated, margin);
                        }
                    } else {
                        // Check for required options and input
                        if( (id.do_eliminate != 1) || (eliminate == 0) )
                        {
                            return 1;
                        }
                        
                        enter_all_phases();
                        darray2[0] = 0.0;
                        tqce(" ", 0, 0, darray2, &noerr);
                        count_amounts(total_amount);
                        get_amounts(amounts);
                        /*
                            TODO remove, only for debugging
                        */
                        // tqshow(&noerr);
                        // table();
                        // getchar();
                        
                        eliminate_phases(eliminate);
                        darray2[0] = 0.0;
                        tqce(" ", 0, 0, darray2, &noerr);
                        count_amounts(total_amount_with_eliminations);
                        get_amounts(amounts_with_eliminations);
                        /*
                            TODO remove, only for debugging
                        */
                        // tqshow(&noerr);
                        // table();
                        // getchar();
                        
                        if (biggest_error_calc(amounts, amounts_with_eliminations, biggest_error) == 1) {
                            printf("\nElimination lead to an error for this state\n");
                            tqshow(&noerr);
                            puts("");
                            for(int i = 0; i < nphases; ++i)
                            {
                                printf("%d: %14.5f\n", i, calc_error(amounts[i], amounts_with_eliminations[i]));
                            }
                            /*
                                TODO remove, only for debugging
                            */
                            // getchar();
                        }
                    }
                }

                next(loop, nelements, 0, step);
            }
        }
    }

    // current time after iteration
    then = time(NULL);
    
    show_amounts(total_amount);

    if (id.do_calc_errors == 1) {
        
        printf("\n\nAmounts with elimination:\n");
        show_amounts(total_amount_with_eliminations);
        
        for(int i = 0; i < nphases; ++i)
        {
            total_amount_with_eliminations[i] = fabs((total_amount[i] - total_amount_with_eliminations[i])) / total_amount[i];
        }
        
        printf("\n\nTotal Errors:\n");
        show_amounts(total_amount_with_eliminations);
        
        printf("\n\nBiggest Errors:\n");
        show_amounts(biggest_error);
    }

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
    (*od).total_errors = total_amount_with_eliminations;
}

char* error_code_to_str(int error_code) {
    switch (error_code) {
        case 1:
        return "Error Code 1: do_eliminate and eliminate must be set for do_calc_errors as well!";
        break;
    }
}

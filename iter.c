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

int check_input(struct iteration_input*);
void next(int[], int, int, int, int*, int);

// ========================================
// = Runs Iterations with the given input =
// = May return an error code             =
// ========================================
int run_iteration(struct iteration_input id, struct iteration_output* od) {

    int rtn_val = check_input(&id);
    if (rtn_val) {
        return rtn_val;
    }

    /* TODO: debugging */
    // printf("bla: %li\n", id.ignored_elements);

    int t_min = id.t_min;
    int t_max = id.t_max;
    int p_min = id.p_min;
    int p_max = id.p_max;
    int step  = id.step;
    double margin = id.margin;
    LI numcon;
    DB darray2[2];
    LI noerr;
    int* eliminated_phases = id.eliminated_phases;
    int* ignored_elements = id.ignored_elements;

    int now, then, sum;
    LI nphases, nelements;

    /* number of elements */
    tqnosc(&nelements,&noerr);
    /* number of phases */
    tqnop(&nphases, &noerr);

    /* will be 1 for all phases not having any amount */
    int *op_elim_phases = malloc(nphases * sizeof(int));

    /* containing the initial amounts of the elements */
    int *loop = malloc(nelements * sizeof(int));

    /* will contain the total amount of any phase */
    DB *total_amount = malloc(nphases * sizeof(DB));

    /* will contain the amounts for that state */
    DB *amounts = malloc(nphases * sizeof(DB));

    /* will contain the amounts for that state with the eliminated phases */
    DB *amounts_with_eliminations = malloc(nphases * sizeof(DB));

    /* will contain the total amount of any phase for the equilibrium with eliminated phases */
    DB *total_amount_with_eliminations = malloc(nphases * sizeof(DB));

    /* will contain the biggest error a phase had */
    DB *biggest_error = malloc(nphases * sizeof(DB));

    /* initializing arrays to zero */
    for (int i = 0; i < nphases; ++i) {
        op_elim_phases[i] = 0 ;
        total_amount[i] = 0;
        amounts[i] = 0;
        total_amount_with_eliminations[i] = 0;
        amounts_with_eliminations[i] = 0;
        biggest_error[i] = 0;
    }

    if (id.do_eliminate == 1) {
        eliminate_phases(eliminated_phases);
    }

    /* current time before iteration */
    now = time(NULL);

    for (int i = t_min; i <= t_max; ++i)
    {
        tqsetc("T", 0, 0, i, &numcon, &noerr);

        for (int j = p_min; j <= p_max; ++j)
        {
            tqsetc("P", 0, 0, j, &numcon, &noerr);

            /* Display present settings if do_tqshow == 1 */
            if (id.do_tqshow == 1) {
                printf("\n\n**** Begin output table produced by tqshow\n");
                fflush(NULL);
                tqshow(&noerr);
                fflush(NULL);
                printf("\n**** End output table produced by tqshow\n\n\n");
                /* wait for enter to continue */
                getchar();
            }

            for (int i = 0; i < nelements; ++i)
            {
                loop[i] = 0;
            }

            while (loop[0] <= step) {

                sum = sum_array(loop, nelements);

                if (sum == step) {
                    if ( (id.do_set_ranges == 1) && (check_for_range(loop, id.min_set_ranges, id.max_set_ranges, nelements) == 0) ) {

                        /* TODO: debugging */
                        /*
                        puts("Omitted:");
                        for (int d = 0; d < nelements; d++) {
                            printf("%i ", loop[d]);
                        }
                        puts("");
                        */

                        next(loop, nelements, 0, step, ignored_elements, nelements);

                        continue;
                    }

                    /* set initial amounts for all phases */
                    set_all_ia(loop, step, ignored_elements);

                    if (id.do_calc_errors == 0)
                    {
                        darray2[0] = 0.0;
                        tqce(" ", 0, 0, darray2, &noerr);
                        if (noerr) abortprog(__LINE__,"tqce",noerr, __FILE__);

                        count_amounts(total_amount);

                        if (id.do_eliminate == 0) {
                            check_elimination(op_elim_phases, margin);
                        }
                    } else {

                        enter_all_phases();
                        darray2[0] = 0.0;
                        tqce(" ", 0, 0, darray2, &noerr);
                        count_amounts(total_amount);
                        get_amounts(amounts);
                        /* TODO remove, only for debugging */
                        // tqshow(&noerr);
                        // table();
                        // getchar();

                        eliminate_phases(eliminated_phases);
                        darray2[0] = 0.0;
                        tqce(" ", 0, 0, darray2, &noerr);
                        count_amounts(total_amount_with_eliminations);
                        get_amounts(amounts_with_eliminations);
                        /* TODO remove, only for debugging */
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
                            /* TODO remove, only for debugging */
                            // getchar();
                        }
                    }
                }

                next(loop, nelements, 0, step, ignored_elements, nelements);
            }
        }
    }

    /* current time after iteration */
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

    /* Print ChemSage output table */
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
    (*od).eliminated_phases = op_elim_phases;
    (*od).total_errors = total_amount_with_eliminations;

    return 0;
}


// =================================================================================
// = Checks the input array and returns an error code if it detects anything funny =
// =================================================================================
int check_input(struct iteration_input* id) {
    LI nelements, noerr;
    
    tqnosc(&nelements,&noerr);

    /* if do_ignore_elements is set */
    if ( (*id).do_ignore_elements == 1 ) {
        /* but ignored_elements[] isn't, return error code */
        if ( (*id).ignored_elements == 0 ) {
            return 2;
        }
    } else {
        /* ensure that ignored_elements[] is 0 */
        (*id).ignored_elements = 0;
    }


    /* if do_calc_errors is set */
    if( (*id).do_calc_errors == 1 ) {
        /* but eliminate[] isn't, return error code */
        if ( (*id).eliminated_phases == 0  ) {
            return 1;
        }
    } else {
        /* ensure that eliminate[] is 0 */
        (*id).eliminated_phases = 0;
    }


    /* if do_eliminate is set */
    if ( (*id).do_eliminate == 1 ) {
        /* but eliminate[] isn't, ... */
        if ( (*id).eliminated_phases == 0 ) {
            return 3;
        }
    } else {
        /* ensure that eliminate[] is 0 */
        (*id).eliminated_phases = 0;
    }


    /* if do_set_ranges is set */
    if ( (*id).do_set_ranges == 1 ) {
        /* but [min/max]_set_ranges[] isn't, ... */
        if ( ( (*id).min_set_ranges == 0 ) || ( (*id).max_set_ranges == 0 ) ) {
            return 4;
        }
        /* check if all min ranges are below or equal to max ranges */
        for (int i = 0; i < nelements; i++) {
            if ((*id).min_set_ranges[i] > (*id).max_set_ranges[i]) {
                return 5;
            }
        }
        
    } else {
        /* ensure [min/max]_set_ranges[] is 0 */
        (*id).min_set_ranges = 0;
        (*id).max_set_ranges = 0;
    }

    return 0;

}

// =======================================================
// = Returns a string that explains the given error code =
// =======================================================
char* error_code_to_str(int error_code) {

    switch (error_code) {
    case 1:
        return "Error Code 1: eliminate[] must be set for do_calc_errors!";
        break;
    case 2:
        return "Error Code 2: ignored_elements[] must be set for do_ignore_elements!";
        break;
    case 3:
        return "Error Code 3: eliminate[] must be set for do_eliminate!";
        break;
    case 4:
        return "Error Code 4: [min/max]_ignored_ranges[] must be set for do_set_ranges!";
        break;
    case 5:
        return "Error Code 5: [min/max]_ranges do not match. min ranges must be smaller of equal to max ranges";
        break;
    }

    return "";

}

// ============================================================================
// = Returns the next distribution for an equilibrium skipping ignored phases =
// ============================================================================
void next(int arr[], int size, int lower_bound, int upper_bound, int* ignored_elements, int nelements) {
    int pointer = size - 1;

    if (pointer < 0)
    {
        arr[0]++;
    }
    if ( check_for_ignored_element(pointer, ignored_elements) )
    {
        if (pointer == 0) {
            /* if the first element is to be ignored we have to set it        */
            /* to uppper_bound + 1 to break the while loop in run_iteration() */
            arr[0] = upper_bound + 1;
            return;
        } else {
            /* skip element if it should be ignored */
            pointer--;
        }
    }
    if ((arr[ pointer ] >= upper_bound) && (pointer > 0)) {
        arr[pointer] = lower_bound;
        next(arr, pointer, lower_bound, upper_bound, ignored_elements, nelements);
    } else {
        arr[pointer]++;
    }
}

// ==============================================
// = Prints the settings set in iteration_input =
// ==============================================
void print_settings(struct iteration_input id, int nelements, int nphases) {

    /* clean settings from ignored input (especially the arrays) */
    check_input(&id);

    puts("**********************");
    puts("* Iteration Settings *");
    puts("**********************");

    printf("%-30s%i\n", "Start Temperature:", id.t_min);
    printf("%-30s%i\n", "Stop Temperature:", id.t_max);
    printf("%-30s%i\n", "Start Pressure:", id.p_min);
    printf("%-30s%i\n", "Stop Pressure:", id.p_max);

    printf("%-30s%i\n", "Step:", id.step);
    printf("%-30s%lf\n", "Margin:", id.margin);

    printf("%-30s%i\n", "do_tqshow:", id.do_tqshow);
    printf("%-30s%i\n", "do_tqcenl:", id.do_tqcenl);
    printf("%-30s%i\n", "do_table:", id.do_table);
    printf("%-30s%i\n", "do_eliminate:", id.do_eliminate);

    printf("%-30s%i\n", "do_ignore_elements:", id.do_ignore_elements);
    printf("%-30s%i\n", "do_set_ranges:", id.do_set_ranges);
    printf("%-30s%i\n", "do_test:", id.do_test);

    printf("%-30s%i\n", "do_calc_errors:", id.do_calc_errors);

    if (id.eliminated_phases == 0) {
        printf("%-30s%i\n", "eliminated_phases:", id.eliminated_phases);
    } else {
        printf("%-30s", "eliminated_phases:");

        for (int i = 0; i < nphases; i++) {
            printf("%i ", id.eliminated_phases[i]);
        }


        printf("\n");
    }

    if (id.ignored_elements == 0) {
        printf("%-30s%i\n", "ignored_elements:", id.ignored_elements);
    } else {
        printf("%-30s", "ignored_elements:");

        for (int i = 0; i < nelements; i++) {
            printf("%i ", id.ignored_elements[i]);
        }


        printf("\n");
    }

    if (id.min_set_ranges == 0) {
        printf("%-30s%i\n", "min_set_ranges:", id.min_set_ranges);
    } else {
        printf("%-30s", "min_set_ranges:");

        for (int i = 0; i < nelements; i++) {
            printf("%i ", id.min_set_ranges[i]);
        }


        printf("\n");
    }

    if (id.max_set_ranges == 0) {
        printf("%-30s%i\n", "max_set_ranges:", id.max_set_ranges);
    } else {
        printf("%-30s", "max_set_ranges:");

        for (int i = 0; i < nelements; i++) {
            printf("%i ", id.max_set_ranges[i]);
        }


        printf("\n");
    }

    printf("\n\n\n");

}

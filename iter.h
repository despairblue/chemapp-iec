//
//  iter.h
//
//
//  Created by Danny Arnold on 2011-07-20.
//  Copyright 2011 TU Bergakademie Freiberg. All rights reserved.
//


#include "lib64/cacint.h"

// ===================
// = iteration_input =
// ================================================
// = Takes all arguments needed for the iteration =
// ================================================
struct iteration_input {
    int t_min;                  // temp to start with
    
    int t_max;                  // temp to stop with
    
    int p_min;                  // pressure to start with
    
    int p_max;                  // pressure to stop with

    int step;                   // (1/step) == increment
    
    double margin;              // how off can a result be from zero
                                // to still be assumed zero

    int do_tqshow;              // call tqshow() after each iteration
    
    int do_tqcenl;              // call tqcenl() instead of tqce()
    
    int do_table;               // call table() after each iteration
    
    int do_eliminate;           // eliminate phases in
                                // eliminate[]
    
    int do_ignore_elements;     // ignore elements in ignored_elements[]
    
    int do_set_ranges;          // only calculate ranges set in min/max_set_ranges[]
    
    int do_test;                // no idea what for

    int do_calc_errors;         // calculate average error
                                // implies do_eliminate == 1

    int* eliminated_phases;     // length: number of phases
                                // 0 means the phase will be eliminated
                                
    int* ignored_elements;      // length: number of elements
                                // 0 means the element will be ignored
                                
    int* min_set_ranges;        // length: number of elements
    
    int* max_set_ranges;        // length: number of elements
                                // Ex.: min_ignored_ranges = {2, 6}
                                // Ex.: max_ignored_ranges = {4, 7}
                                // step = 10
                                // only equilibriums for e1 between 0.2 and 0.4
                                // and e2 between 0.6 and 0.7 will be solved
};

#define ITERATION_INPUT_DEFAULT {0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

struct iteration_output {
    LI time_taken;              // outputs the time needed for calculating the equilibriums
    
    int* eliminated_phases;     // array of all phases that could be eliminated
                                // (see iteration_input.eliminated_phases)
    
    double* max_errors;         // biggest error that occurred by eliminating phases
    
    double* total_errors;       // total error that occurred by eliminating phases
};

#define ITERATION_OUTPUT_DEFAULT {0, 0, 0, 0};

int run_iteration(struct iteration_input, struct iteration_output*);
char* error_code_to_str(int);
void print_settings(struct iteration_input, int, int);

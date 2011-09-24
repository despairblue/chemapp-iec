//
//  iter.h
//
//
//  Created by Danny Arnold on 2011-07-20.
//  Copyright 2011 TU Bergakademie Freiberg. All rights reserved.
//


#include "lib64/cacint.h"

// ==================
// = iteration_data =
// ==================
/*
	Takes all arguments needed for the iteration
*/
struct iteration_input {
    int t_min;
    int t_max;
    int p_min;
    int p_max;

    int step;
    double margin;

    int do_tqshow;
    int do_tqcenl;
    int do_table;
    int do_eliminate;
    int do_test;
    // do_eliminate has to be set to 1 as well and eliminate too!
    int do_calc_errors;

    int* eliminate;
};

#define ITERATION_INPUT_DEFAULT {0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0};

struct iteration_output {
    LI time_taken;
    int* eliminated;
    double* max_errors;
    double* total_errors;
};

int run_iteration(struct iteration_input, struct iteration_output*);
char* error_code_to_str(int);


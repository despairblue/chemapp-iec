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
    int t_min;              //temp to start with
    int t_max;              //temp to stop with
    int p_min;              //pressure to start with
    int p_max;              //pressure to stop with

    int step;               //(1/step) == increment
    double margin;          //how off can a result be from zero
                            //to still be assumed zero  

    int do_tqshow;          //call tqshow() after each iteration
    int do_tqcenl;          //call tqcenl() instead of tqce()
    int do_table;           //call table() after each iteration
    int do_eliminate;       //eliminate phases in 
                            //eliminate[]
    int do_ignore_elements; //ignore elements in ignored_elements[]
    int do_ignore_ranges;   //
    int do_test;            //no idea what for
    
    int do_calc_errors;     //calculate average error
                            //implies do_eliminate == 1

    int* eliminate;         //length: number of phases
                            //0 means the phase will be eliminated
    int* ignored_elements;  //length: number of elements
                            //0 means the element will be ignored
    int**ignored_ranges;    //AxB: 
                            //A = number of elements
                            //B = 2
                            // Ex.: ignored_ranges = {{2, 4}, {6, 7}}
                            // step = 10
                            // only equilibriums for e1 between 0.2 and 0.4
                            // and e2 between 0.6 and 0.7 will be solved
};

#define ITERATION_INPUT_DEFAULT {0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

struct iteration_output {
    LI time_taken;
    int* eliminated;
    double* max_errors;
    double* total_errors;
};

int run_iteration(struct iteration_input, struct iteration_output*);
char* error_code_to_str(int);


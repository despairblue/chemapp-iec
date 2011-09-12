//
//  helpers.h
//
//
//  Created by Danny Arnold on 2011-07-20.
//  Copyright 2011 TU Bergakademie Freiberg. All rights reserved.
//


#include "cacint.h"

void abortprog( int, char[10], LI );
void table();
void check_elimination(int[], double);
DB calc_error(DB, DB);
int biggest_error_calc(DB[] , DB[] , DB[]);
void get_amounts(DB[]);
void count_amounts(DB[]);
void show_amounts(DB[]);
void enter_all_phases();
void reset_vars(DB*, DB*, DB*, DB*, DB*, DB*);
void set_all_ia(int[], int);
// int contains(int**, DB, DB, DB, int, int);
// void add(int**, DB, DB, DB, int, int);
void eliminate_phases(int[]);

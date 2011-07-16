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

void run_iteration(struct iteration_data);
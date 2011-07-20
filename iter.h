#include "cacint.h"

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
	
	int do_tqshow;
	int do_tqcenl;
	int do_table;
	int do_eliminate;
};

struct iteration_output {
	LI time_taken;
};

void run_iteration(struct iteration_input, struct iteration_output*);
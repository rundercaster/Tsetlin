

#include "TsetlinMachineBits.h"

#define CLASSES 26

struct MultiClassTsetlinMachine { 
	struct TsetlinMachine *tsetlin_machines[CLASSES];
};

struct MultiClassTsetlinMachine *CreateMultiClassTsetlinMachine();

void mc_tm_initialize(struct MultiClassTsetlinMachine *mc_tm);

void mc_tm_initialize_random_streams(struct MultiClassTsetlinMachine *mc_tm, float s);

float mc_tm_evaluate(struct MultiClassTsetlinMachine *mc_tm, unsigned int X[][LA_CHUNKS], int y[], int number_of_examples, int class_acc[], int THRESHOLD);

void mc_tm_fit(struct MultiClassTsetlinMachine *mc_tm, unsigned int X[][LA_CHUNKS], int y[], int number_of_examples, int epochs, float s, int suppress[]);

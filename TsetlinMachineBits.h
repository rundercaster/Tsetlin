

#include "TsetlinMachineBitsConfig.h"

#define INT_SIZE 32

#define LA_CHUNKS (((2*FEATURES-1)/INT_SIZE + 1))
#define CLAUSE_CHUNKS ((CLAUSES-1)/INT_SIZE + 1)

#define FILTER (~(0xffffffff << ((FEATURES*2) % INT_SIZE)))

#define PREDICT 1
#define UPDATE 0

struct TsetlinMachine { 
	unsigned int ta_state[CLAUSES][LA_CHUNKS][STATE_BITS];
	unsigned int clause_output[CLAUSE_CHUNKS];
	unsigned int feedback_to_la[LA_CHUNKS];
	int feedback_to_clauses[CLAUSE_CHUNKS];
};

struct TsetlinMachine *CreateTsetlinMachine();

void tm_initialize(struct TsetlinMachine *tm);

void tm_update(struct TsetlinMachine *tm, unsigned int Xi[], int target, float s, int THRESHOLD);

int tm_score(struct TsetlinMachine *tm, unsigned int Xi[], int THRESHOLD);

int tm_get_state(struct TsetlinMachine *tm, int clause, int la);

int tm_action(struct TsetlinMachine *tm, int clause, int la);

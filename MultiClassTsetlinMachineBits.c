

#include <stdio.h>
#include <stdlib.h>

#include "MultiClassTsetlinMachineBits.h"

/**************************************/
/*** The Multiclass Tsetlin Machine ***/
/**************************************/

/*** Initialize Tsetlin Machine ***/
struct MultiClassTsetlinMachine *CreateMultiClassTsetlinMachine()
{

	struct MultiClassTsetlinMachine *mc_tm;

	mc_tm = (void *)malloc(sizeof(struct MultiClassTsetlinMachine));

	for (int i = 0; i < CLASSES; i++) {
		mc_tm->tsetlin_machines[i] = CreateTsetlinMachine();
	}
	return mc_tm;
}



void mc_tm_initialize(struct MultiClassTsetlinMachine *mc_tm)
{
	for (int i = 0; i < CLASSES; i++) {
		tm_initialize(mc_tm->tsetlin_machines[i]);
	}
}

/********************************************/
/*** Evaluate the Trained Tsetlin Machine ***/
/********************************************/

float mc_tm_evaluate(struct MultiClassTsetlinMachine *mc_tm, unsigned int X[][LA_CHUNKS], int y[], int number_of_examples, int class_acc[], int THRESHOLD)
{
	int errors;
	int max_class;
	int max_class_sum;
  // int class_acc[22] = {0};
  
	errors = 0;
	for (int l = 0; l < number_of_examples; l++) {
		/******************************************/
		/*** Identify Class with Largest Output ***/
		/******************************************/

		max_class_sum = tm_score(mc_tm->tsetlin_machines[0], X[l], THRESHOLD);
		max_class = 0;
		
		// output_digit(X[l]);
		// printf( "Class that needs to be: %d\n\n", y[l]);
		
		// printf( "Class: %d,class acc: %d\n",max_class , max_class_sum);
		
		
		for (int i = 1; i < CLASSES; i++) {	
			int class_sum = tm_score(mc_tm->tsetlin_machines[i], X[l], THRESHOLD);
			// printf( "Class: %d,class acc: %d\n", i, class_sum);
			
			if (max_class_sum < class_sum) {
				max_class_sum = class_sum;
				max_class = i;
			}
		}


    class_acc[2*y[l]+1] += 1; //save total for each class

		if (max_class != y[l]) {
		  class_acc[2*y[l]] += 1; //save errors for each class
			errors += 1;
		}
	}

  
  

	
	return 1.0 - 1.0 * errors / number_of_examples;
}

/******************************************/
/*** Online Training of Tsetlin Machine ***/
/******************************************/

// The Tsetlin Machine can be trained incrementally, one training example at a time.
// Use this method directly for online and incremental training.

int mc_tm_update(struct MultiClassTsetlinMachine *mc_tm, unsigned int Xi[], int target_class, float s, int THRESHOLD, int suppress[])
{

    // Randomly pick one of the other classes, for pairwise learning of class output 
    unsigned int negative_target_class = (unsigned int)CLASSES * 1.0*rand()/((unsigned int)RAND_MAX + 1);
    while (negative_target_class == target_class) {
      negative_target_class = (unsigned int)CLASSES * 1.0*rand()/((unsigned int)RAND_MAX + 1);
    }
  if (suppress[negative_target_class] == 0) {
    tm_update(mc_tm->tsetlin_machines[negative_target_class], Xi, 0, s, THRESHOLD);
  }

  if (suppress[target_class] == 0) { 
    tm_update(mc_tm->tsetlin_machines[target_class], Xi, 1, s, THRESHOLD);
    return 0;
  }
  return 1;
}

/**********************************************/
/*** Batch Mode Training of Tsetlin Machine ***/
/**********************************************/

void mc_tm_fit(struct MultiClassTsetlinMachine *mc_tm, unsigned int X[][LA_CHUNKS], int y[], int number_of_examples, int epochs, float s, int suppress[])
{
  int THRESHOLD = 50;
  
	// for (int epoch = 0; epoch < epochs; epoch++) {
		// Add shuffling here...			
    int i = 0;	
		while ( i < number_of_examples) {
			if (mc_tm_update(mc_tm, X[i], y[i], s, THRESHOLD, suppress) == 1) {
        number_of_examples += 1;
      }
      i++;
		}
	}
// }


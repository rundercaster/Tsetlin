#include "MultiClassTsetlinMachineBits.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define EPOCHS 40//400
#define NUMBER_OF_TRAINING_EXAMPLES 20000
#define NUMBER_OF_TRAINING 10000
#define NUMBER_OF_TEST_EXAMPLES 5000

unsigned int X_train[NUMBER_OF_TRAINING_EXAMPLES][LA_CHUNKS];
int y_train[NUMBER_OF_TRAINING_EXAMPLES];

unsigned int X_test[NUMBER_OF_TEST_EXAMPLES][LA_CHUNKS];
int y_test[NUMBER_OF_TEST_EXAMPLES];

unsigned int X_training_2[NUMBER_OF_TEST_EXAMPLES][LA_CHUNKS];
int y_training_2[NUMBER_OF_TEST_EXAMPLES];

void read_file(void)
{
	FILE * fp;
	char * line = NULL;
	size_t len = 0;

	const char *s = " ";
	char *token = NULL;

	// Training Dataset

	for (int i = 0; i < NUMBER_OF_TRAINING_EXAMPLES; i++) {
		for (int j = 0; j < LA_CHUNKS; j++) {
			X_train[i][j] = 0;
		}
	}

	fp = fopen("letters.txt", "r");
	if (fp == NULL) {
		printf("Error opening\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < NUMBER_OF_TRAINING_EXAMPLES; i++) {
		getline(&line, &len, fp);

		token = strtok(line, s);
		for (int j = 0; j < FEATURES; j++) {
			if (atoi(token) == 1) {
				int chunk_nr = j / INT_SIZE;
				int chunk_pos = j % INT_SIZE;
				X_train[i][chunk_nr] |= (1 << chunk_pos);
			} else {
				int chunk_nr = (j + FEATURES) / INT_SIZE;
				int chunk_pos = (j + FEATURES) % INT_SIZE;
				X_train[i][chunk_nr] |= (1 << chunk_pos);
			}
			token=strtok(NULL,s);
		}
		y_train[i] = atoi(token);
    if (y_train[i] >= 65){//fix for ascii(65=a)
      y_train[i] = atoi(token)-65;
    }
	}
	fclose(fp);

	// Test Dataset I

	for (int i = 0; i < NUMBER_OF_TEST_EXAMPLES; i++) {
		for (int j = 0; j < LA_CHUNKS; j++) {
			X_test[i][j] = 0;
		}
	}

	fp = fopen("lettersTest.txt", "r");
	if (fp == NULL) {
		printf("Error opening\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < NUMBER_OF_TEST_EXAMPLES; i++) {
		getline(&line, &len, fp);

		token = strtok(line, s);
		for (int j = 0; j < FEATURES; j++) {
			if (atoi(token) == 1) {
				int chunk_nr = j / INT_SIZE;
				int chunk_pos = j % INT_SIZE;
				X_test[i][chunk_nr] |= (1 << chunk_pos);
			} else {
				int chunk_nr = (j + FEATURES) / INT_SIZE;
				int chunk_pos = (j + FEATURES) % INT_SIZE;
				X_test[i][chunk_nr] |= (1 << chunk_pos);
			}
			token=strtok(NULL,s);
		}

		y_test[i] = atoi(token);
    if (y_test[i] >= 65){//fix for ascii(65=a)
      y_test[i] = atoi(token)-65;
    }
	}
	fclose(fp);

	// Sample of training dataset for speed

	for (int i = 0; i < NUMBER_OF_TEST_EXAMPLES; i++) {
		for (int j = 0; j < LA_CHUNKS; j++) {
			X_training_2[i][j] = 0;
		}
	}

	fp = fopen("MNISTTrainingSampled.txt", "r");
	if (fp == NULL) {
		printf("Error opening\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < NUMBER_OF_TEST_EXAMPLES; i++) {
		getline(&line, &len, fp);

		token = strtok(line, s);
		for (int j = 0; j < FEATURES; j++) {
			if (atoi(token) == 1) {
				int chunk_nr = j / INT_SIZE;
				int chunk_pos = j % INT_SIZE;
				X_training_2[i][chunk_nr] |= (1 << chunk_pos);
			} else {
				int chunk_nr = (j + FEATURES) / INT_SIZE;
				int chunk_pos = (j + FEATURES) % INT_SIZE;
				X_training_2[i][chunk_nr] |= (1 << chunk_pos);
			}
			token=strtok(NULL,s);
		}
		y_training_2[i] = atoi(token);
    if (y_test[i] >= 65){ //fix for ascii(65=a)
      y_test[i] = atoi(token)-65;
    }
	}
	fclose(fp);
}

void output_digit(unsigned int Xi[])
{
	for (int y = 0; y < 28; y++) {
		for (int x = 0; x < 28; x++) {
			int chunk_nr = (x + y*28) / INT_SIZE;
			int chunk_pos = (x + y*28) % INT_SIZE;

			if ((Xi[chunk_nr] & (1 << chunk_pos)) > 0) {
				printf("▋");
			} else {
				printf(" ");
			}
		}
		printf("\n");
	}
}

//print out accuracy for each class
void print_procentages(int p[], int suppress[]){
  for (int i = 0; i < CLASSES; i++) {
    float aa = 1.0 - 1.0*p[2*i]/p[2*i+1];
    if (suppress[i] == 1) {
      printf("X Class %d: %.2f  (%d/%d)\n",i , aa, p[2*i], p[2*i+1]);
    }else{
      printf("  Class %d: %.2f  (%d/%d)\n",i , aa, p[2*i], p[2*i+1]);
    }
  }
}

void calculate_suppress(int p[], int suppress[], float test_accuracy_avg, int epochs){
  for (int i = 0; i < CLASSES; i++) {
    float aa = 1.0 - 1.0*p[2*i]/p[2*i+1];
    // printf("\n\n%.2f",aa);
    if(aa > test_accuracy_avg){
    // if(aa > test_accuracy_avg && epochs %2 == 0){
      suppress[i] = 1;
      // printf("\n\nsuppress class %d (%f/%f)",i,aa,test_accuracy_avg);
    }
    else{
      suppress[i] = 0;
    }
  }
}

int THRESHOLD = 40;

int main(void)
{	
	srand(time(NULL));

	read_file();


  //pick random example from training set
	// int example = (int)(NUMBER_OF_TEST_EXAMPLES-1) * 1.0*rand()/RAND_MAX;
	// printf("\nExample of Digit %d\n\n", y_test[example]);
	// output_digit(X_test[example]);

	struct MultiClassTsetlinMachine *mc_tm = CreateMultiClassTsetlinMachine();

// print info
  printf("\n Threshold: %d",THRESHOLD);
  printf("\n Number of Clauses: %d\n",CLAUSES);
  int train_acc[2*CLASSES+1] = {0}; 
  int test_acc[2*CLASSES+1]  = {0};
  int suppress[CLASSES+1]  = {0}; // 0 is not suppressed 
  float save_accuracy[EPOCHS +1] = {0};

	for (int i = 0; i < EPOCHS; i++) {
		printf("\nEPOCH %d\n", i+1);
		clock_t start_total = clock();
		mc_tm_fit(mc_tm, X_train, y_train, NUMBER_OF_TRAINING, i, S, suppress);
		clock_t end_total = clock();
		double time_used = ((double) (end_total - start_total)) / CLOCKS_PER_SEC;

		printf("Training Time: %.1f s\n", time_used);

		start_total = clock();
		float test_accuracy_avg = mc_tm_evaluate(mc_tm, X_test, y_test, NUMBER_OF_TEST_EXAMPLES, train_acc, THRESHOLD) ;
    save_accuracy[i] = test_accuracy_avg;

    printf("\n\nTrain class accuracy:\n");
    print_procentages(train_acc, suppress);
    
		end_total = clock();
		time_used = ((double) (end_total - start_total)) / CLOCKS_PER_SEC;

		printf("Evaluation Time: %.1f s\n", time_used);
		printf("Train Accuracy: %.1f\n", 100*test_accuracy_avg);

		// float training_2_accuracy = mc_tm_evaluate(mc_tm, X_training_2, y_training_2, NUMBER_OF_TEST_EXAMPLES, test_acc, THRESHOLD);
    // save_accuracy[i] = training_2_accuracy;


    // printf("\n\nTest class accuracy:\n");
    // print_procentages(test_acc, suppress);
		// printf("Test Accuracy: %.1f\n", 100*training_2_accuracy);

    // printf("\n\n progress list:\n");
    for (int j = 0; save_accuracy[j] != 0; j++) {
      // printf("%d epoch: %.4f%%\n",j, save_accuracy[j]);
      printf("%.4f%%\n", save_accuracy[j]);
    }

    calculate_suppress(train_acc, suppress, test_accuracy_avg, i);

	}

	return 0;
}

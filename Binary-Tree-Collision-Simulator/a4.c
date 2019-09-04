#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

// initializing variables
#define N 1024
#define L 10
int K = 3;
int Start = 1;

// declaring topology representations
char readyStations[N][1024];
char binaryTree[L+1][1040][1040];

// initializing counters
int idle = 0;
int success = 0;
int probes = 0;
int collisions = 0;

void probe(int level, int from, int too);
void generateTree(int level, char prefix[], int from);
int isPrefix(char i_integer[], char i_prefix[]);
int toBinary(int k);
int isInArray(int value, int array[], int size);

int main() {
	// User defined settings simulator
	generateTree(1, "", 0);
	int seed = 123456;
	int alreadyPicked[1030];
	for (int j = 0; j < K; j++){
	// generate random integer between 0 and N
		 seed++;
		 int randint = 0;
		 int max_index = N;
		 srandom(seed);
		 while(randint == 0) {
			randint = random();
			randint = randint % (max_index+1);
			if (isInArray(randint, alreadyPicked, j)) {
				randint = 0;
			}		
		}
		//printf("randint is %d\n", randint);
		//printf("setting %d in the tree that is %s\n", j, binaryTree[L][randint]);
		// convert it to binary
		// add it to readyStations at jth index
		alreadyPicked[j] = randint;
		printf("Station %s is ready\n", binaryTree[L][randint]);
		strcpy(readyStations[j], binaryTree[L][randint]);
	}

	probe(Start, 0, pow(2, Start) - 1);
	printf("idle is %d\n", idle);
	printf("success is %d\n", success);
	printf("probes is %d\n", probes);
	printf("collisions is %d\n", collisions);
	
	// Generator for all possible outcomes
	seed = 0;
	success = 0;
	probes = 0;
	idle = 0;
	collisions = 0;
	for (int j = 0; j <= 10; j = j + 2) {
		Start = j;
		printf("Using Start = %d\n", j);
		printf("K, Probes, Successful, Idle, Collisions.\n");
		// start probing at the givin starting point, from beginning to end
		for (int k_ins = 0; k_ins <= 10; k_ins++) {
			//int seed = 0;
			K = pow(2, k_ins);
			//printf("Checking K = %d\n", K);
			int total_success = 0;
			int total_probes = 0;
			int total_idle = 0;
			int total_collisions = 0;
			for (int i = 0; i < 100; i++){
				int alreadyPicked[1030];
				for (int j = 0; j < K; j++){
					// generate random integer between 0 and N
					seed++;
		 			int randint = 0;
		 			int max_index = N;
		 			srandom(seed);
		 			while(randint == 0) {
						randint = random();
						randint = randint % (max_index+1);
						if (isInArray(randint, alreadyPicked, j)) {
							randint = 0;
						}		
					}
					//printf("randint is %d\n", randint);
					//printf("setting %d in the tree that is %s\n", j, binaryTree[L][randint]);
					// convert it to binary
					// add it to readyStations at jth index
					alreadyPicked[j] = randint;
					strcpy(readyStations[j], binaryTree[L][randint]);
				}
			
				probe(Start, 0, pow(2, Start) - 1);
				//printf("completed probe\n");
				total_success = total_success + success;
				total_probes = total_probes + probes;
				total_idle = total_idle + idle;
				total_collisions = total_collisions + collisions;
				//printf("K is %d\n", K);
				//printf("idle is %d\n", idle);
				//printf("success is is %d\n", success);
				//printf("probes is is %d\n", probes);
				//printf("collisions is is %d\n", collisions);
				success = 0;
				probes = 0;
				idle = 0;
				collisions = 0;

			}

			total_success = total_success / 100;
			total_probes = total_probes / 100;
			total_idle = total_idle / 100;
			total_collisions = total_collisions / 100;
			
			float percentage_success = (float) (((float) K / (float) total_probes) * 100);
			float percentage_idle = (float) (((float) total_idle / (float) total_probes) * 100);
			float percentage_collisions = (float) (((float) total_collisions / (float) total_probes) * 100);
			//printf("K = %d, Probes = %d, Success = %f Percent, Idle = %f Percent, Collisions = %f Percent.\n", K, total_probes, percentage_success, percentage_idle, percentage_collisions);
			printf("%d, %d, %f, %f, %f.\n", K, total_probes, percentage_success, percentage_idle, percentage_collisions);
		}
	}
	
}
void probe(int level, int from, int too) {
	//increment number of probes used
	// if the level is 0 and we have more than 1 ready to send, collision detected, go to next level
	if (level == 11) {
		printf("there was a collision at level 10");
		return;
	}
	if (level == 0 && N > 1) {
		collisions++;
		probe(1, 0, 1);
		probes = probes + 1;
	}
	// if the level is greater than 0 do the following
	else {
		// loop through each binary prefix in the binary tree from 'from' to 'too'. 
		for (int i = from; i <= too; i++){
			// loop through each ready station and count the number of matches to the selected prefix
			probes++;
			int count = 0;
			for (int j = 0; j < K; j++){
				if (isPrefix(readyStations[j], binaryTree[level][i])){
					count++;
				}
			}
			// if 0 match then this is an idle probe
			if (count == 0) {
				idle++;
			}
			// if 1 matches then we have a success
			if (count == 1) {
				success++;
			}
			// if more than 1 matches then we have a collision, check the sub nodes/leafs at the next level for successes
			if (count > 1) {
				collisions++;
				probe(level + 1, i*2, i*2+1);
			}
		}
	}
}

void generateTree(int level, char prefix[], int from) {
	// functon that generates the binary tree
	if (level == L + 1) {
		//printf("prefix is %s\n", prefix);
		return;	
	}
	
	char dest[1024];
	strcpy(dest, prefix);
	char newchar[1024];
	strcat(dest, "0");
	//printf("putting %s into binary try at %d, %d\n", dest, level, from);
	strcpy(binaryTree[level][from], dest);
	if (level < L + 1){ generateTree(level + 1, dest, from*2); }
	
	strcpy(dest, prefix);
	strcat(dest, "1");
	//printf("putting %s into binary try at %d, %d\n", dest, level, from + 1);
	strcpy(binaryTree[level][from + 1], dest);
	if (level < L + 1){ generateTree(level + 1, dest, (from + 1)*2); }
}

// function that checks if 1 integer is a prefix to another
int isPrefix(char i_integer[], char i_prefix[]) {

	return strncmp(i_prefix, i_integer, strlen(i_prefix)) == 0;
}

// functon that checks if an element is a member of an array
int isInArray(int value, int array[], int size) {
    for (int i = 0; i < size; i++) {
	if (value == array[i]) {
		return 1;
	}
    }
    return 0;
}

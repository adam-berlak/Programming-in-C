#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>

int frame_size;
int sequence[5000];

int getPosition(int value, int array[], int from, int too);

int main( int argc, char ** argv) {
	FILE * fPointer;
	fPointer = fopen(argv[2], "r");
	
	char s_sequence[1024];
	fgets(s_sequence, 1024, fPointer);
	s_sequence[1023] = '\0';
	
	int i = 0;
	int size = 0;
	char token[1024];
	sequence[i] = atoi(strtok(s_sequence, " ")); 
	size++;
	int test = 1;
	while( test ) {
		strcpy(token, strtok(NULL, " "));
		if (strlen(token) == 2) { test = 0; }
		//printf( "Token is %d\n", atoi(token) );
		sequence[size] = atoi(token); 
		size++;
	}

	char s_frame_size[1024];
	strcpy(s_frame_size, argv[1]);
	frame_size = atoi(s_frame_size);
	int frame[frame_size];
	for (int i = 0; i < frame_size; i++){
		frame[i] = 9999;
	}
	
	// Setting up queue
	int queue[10000];
	int queue_front = 0;
	int queue_end = frame_size - 1;
	for (int i = 0; i < frame_size; i++){
		queue[i] = i;
	}

	// Implementing LRU
	int page_faults = 0;
	for (i = 0; i < size; i++){
		// if the element is not found in the frame do the following
		if (getPosition(sequence[i], frame, 0, frame_size) == 999) {
			frame[queue[queue_front]] = sequence[i];
			queue_front++;
			queue_end++;
			queue[queue_end] = queue_front - 1;
			queue[queue_front - 1] = 999;
			page_faults++;
		}
		// if the element is found in the frame do the following
		if (getPosition(sequence[i], frame, 0, frame_size) != 999){
			int element_loc = getPosition(sequence[i], frame, 0, frame_size);
			int queue_loc = getPosition(element_loc, queue, queue_front, queue_end + 1);
			// move everything at and after queue_loc to the left
			for (int j = queue_loc; j < queue_end; j++){
				queue[j] = queue[j + 1];
			}
			// set the element_loc to be the last element in the queue
			queue[queue_end] = element_loc;
		}
	}
	// Print Result
	printf("LRU:\n - frames: ");
	for (i = 0; i < frame_size; i++) {
		printf("%d ", frame[i]);
	}
	printf("\n - page faults: %d\n", page_faults);

	// Resetting frame
	for (i = 0; i < frame_size; i++){
		frame[i] = 9999;
	}
	
	// --------------------------------------------------------------------------------------------------------------------------------
	
	// Implementing Optimal
	page_faults = 0;
	int count = 0;
	for (i = 0; i < size; i++){
		// if the frame is not filled yet and the element is not in the frame yet do the following
		if (count < frame_size && getPosition(sequence[i], frame, 0, frame_size) == 999) {
			frame[count] = sequence[i];
			count++;
			page_faults++;
		}
		// if the element is not found in the frame do the following
		else if (getPosition(sequence[i], frame, 0, frame_size) == 999) {
			int least_used = 0;
			int least_used_distance = 0;
			// find the page that will not be used for the longest period of time
			for (int j = 0; j < frame_size; j++){
				if (getPosition(frame[j], sequence, i, size) > least_used_distance) {
					least_used = j;
					least_used_distance = getPosition(frame[j], sequence, i, size);
				}
			}
			// replace that page
			frame[least_used] = sequence[i];
			
			page_faults++;
		}
	}
	
	// Print Result
	printf("Optimal:\n - frames: ");
	for (i = 0; i < frame_size; i++) {
		printf("%d ", frame[i]);
	}
	printf("\n - page faults: %d\n", page_faults);
	
	// Resetting frame
	for (i = 0; i < frame_size; i++){
		frame[i] = 9999;
	}
	
	// --------------------------------------------------------------------------------------------------------------------------------
	
	// Implementing Clock
	page_faults = 0;
	int ref_bits[frame_size];
	int pointer = 0;
	for (i = 0; i < frame_size; i++){
		ref_bits[i] = 0;
	}
	for (i = 0; i < size; i++){
		// if element is not in the frame and the bit at the pointer is 0, replace the element at that frame and increment the pointer
		if (getPosition(sequence[i], frame, 0, frame_size) == 999 && ref_bits[pointer] == 0) {
			frame[pointer] = sequence[i];
			ref_bits[pointer] = 1;
			page_faults++;
			if (pointer == frame_size - 1) {
				pointer = 0;
			}
			else {
				pointer++;
			}
		}
		// if element is not in the frame and the bit at the pointer is 1, move to the next element and set the bit to 0
		else if (getPosition(sequence[i], frame, 0, frame_size) == 999 && ref_bits[pointer] == 1) {
			ref_bits[pointer] = 0;
			if (pointer == frame_size - 1) {
				pointer = 0;
			}
			else {
				pointer++;
			}
			i =  i - 1;
		}
		// if element is already in the frame, and if its corresponding bit is 0 set it to 1
		else if (getPosition(sequence[i], frame, 0, frame_size) != 999) {
			int index = getPosition(sequence[i], frame, 0, frame_size);
			if (ref_bits[index] == 0) ref_bits[index] = 1;
		}
	}
	
	// Print Result
	printf("Clock:\n - frames: ");
	for (i = 0; i < frame_size; i++) {
		printf("%d ", frame[i]);
	}
	printf("\n - page faults: %d\n", page_faults);
/*
*/
}

int getPosition(int value, int array[], int from, int too){
	for (int i = from; i < too; i++){
		//printf("int %d is %d\n", i, array[i]);
		if (array[i] == value){
			return i;
		}
	}
	return 999;
}

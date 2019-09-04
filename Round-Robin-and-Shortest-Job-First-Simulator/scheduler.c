#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
int size = 0;
int roundRobin(int config[][2], int time_quan, int size);
int shortestJobFirst(int config[][2], int size);
int main( int argc, char ** argv) {
  printf("reached here\n");

  FILE * fPointer;
  fPointer = fopen(argv[1], "r");
  
  char type[1024];
  strcpy(type, argv[2]);

  char processes[50][50];
  int processes_numeric[50][2];
  int i = 0;

  while(!feof(fPointer)){  
    if (fgets(processes[i], 10, fPointer) == NULL) break;
	printf("number %d is %s\n", i, processes[i]);

	size++;
	i++;
  }

  for( int i = 0; i < size; i++) {
	  char string[1024];
	  strcpy(string, processes[i]);
	  string[1023] = '\0';
	  processes_numeric[i][0] = atoi(strtok(string, " "));
	  processes_numeric[i][1] = atoi(strtok(NULL, " "));
  }

  if (strstr(type, "RR") != NULL) {
	  int time_quan = atoi(argv[3]);
	  roundRobin(processes_numeric, time_quan, size);
  }

  else if (strstr(type, "SJF") != NULL) {
	  shortestJobFirst(processes_numeric, size);
  }

  fclose(fPointer);

  return 1;
}

int roundRobin(int config[][2], int time_quan, int size) { 
  // create queue array, make time initialize to process 1 time, and put process 1 in the queue. For every time unit loop through config to find processes wth same time and add it to the queue

  int time = 0;
  int amount = time_quan;

  int queueEnd = 0;
  int queueFront = 0;
  int queue[50];
  
  int maxTime = config[0][0];
  for (int i = 0; i < size; i++){
	  maxTime = maxTime + config[i][1];
  }
  printf("Time ");
  for (int i = 0; i < size+1 - 1; i++) {
	printf("P%d ", i);
  }
  printf("\n");
  printf("---------------------------------------------------------\n");
  while (time <= maxTime) {
	  // add all proccesses that start at the givin time to the end of the queue
	  if (time < config[0][0]) {
		printf("  %d\n", time);
		time++;
		continue;
	  }
	  for (int i = 0; i < size+1 - 1; i++) { 
		  if (config[i][0] == time) {
			  queue[queueEnd] = i;
			  queueEnd++;
		  }
		  if (config[i][0] > time) break;
		
	  }
	 
	  // execute next process in queue 
	  if (config[queue[queueFront]][1] > 0) {

		
		  amount--;

		  config[queue[queueFront]][1]--;
		
		  // print to command line
		  printf("  %d  ", time);
		  for (int i = 0; i < size; i++){
			  if (i == queue[queueFront]) {
				  printf(" . ");
			  }
			  else if (config[i][0] <= time) {
				  printf(" + ");
			  }
		  }
		  printf("\n");
	  }

	  // if process has finshed running move the queue, but only if there is another process in the queue, otherwise repeat
	  if (config[queue[queueFront]][1] == 0) {
          	queueFront++;
		  amount = time_quan;
	  }
	  // if proccess has used up the time quantum but it is not finished, put it at the end of the queue
	  if (amount == 0 && config[queue[queueFront]][1] > 0) {
		  queue[queueEnd] = queue[queueFront];
		  queueFront++;
		  queueEnd++;
		  amount = time_quan;
	  }

	  time++;
  }
  
  
  return 1;
}

int shortestJobFirst(int config[][2], int size) {
	
  int time = 0;

  int queueEnd = 0;
  int queueFront = 0;
  int queue[50];
  int active = 0;
  
  int maxTime = config[0][0];
  for (int i = 0; i < size; i++){
	  maxTime = maxTime + config[i][1];
  }

  printf("Time ");
  for (int i = 0; i < size+1 - 1; i++) {
	printf("P%d ", i);
  }
  printf("\n");
  printf("---------------------------------------------------------\n");
  
  while (time <= maxTime) {
	  // add all proccesses that start at the givin time to the queue
	  int spot;
	  if (time < config[0][0]) {
		printf("  %d\n", time);
		time++;
		continue;
	  }
	  for (int i = 0; i < size + 1; i++) { 
		  if (config[i][0] == time) {
			//printf("i is %d, and it has time %d\n", i, config[i][0]);
			
			  // shift everything with longer burst time than it to the right
			  for (int j = 0; j < queueEnd; j++){
				  //printf("queue end is %d\n", queueEnd);
				  spot = queueEnd;
				  if (config[queue[j]][1] > config[i][1] && !(j == queueFront && active == 1)) {
					//printf("setting spot to j\n");
					  spot = j;
					  for (int k = queueEnd + 1; k >= j; k--){   
						 
						  if (k == queueFront && active == 1){
							//printf("got here\n");
							  break;
						  }
						  else {
							  queue[k]=queue[k-1];
						  }
						
					  }
					  break;
					
				  }
			  }
			
			  // add process to the queue
			if (!(spot == queueFront && active == 1)){
			  queue[spot] = i;
			  queueEnd++;
				for (int i = 0; i < 3; i++) {
					//printf("%d", queue[i]);
				}
			}
			
			
		  }
		  if (config[i][0] > time) break;
	  }

	  // execute next process in queue 
	  if (config[queue[queueFront]][1] > 0) {
		active = 1;
		  config[queue[queueFront]][1]--;
		  // print to command line
		  printf("  %d  ", time);
		  for (int i = 0; i < size; i++){
			  if (i == queue[queueFront]) {
				  printf(" . ");
			  }
			  else if (config[i][0] <= time) {
				  printf(" + ");
			  }
		  }
		  printf("\n");
	  }

	  // if process has finshed running move the queue, but only if there is another process in the queue, otherwise repeat
	  if (config[queue[queueFront]][1] == 0) {
          	queueFront++;
		 active = 0;
	  }


	  time++;

  }

  
  return 1;
}

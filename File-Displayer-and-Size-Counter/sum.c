#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>

#define MAX_FNAME_SIZE 512
#define MAX_FILES 1024

int add = 0;
int sums[100000000];
void * sum(void * u){
  int *array = (int*) u;
  int size = sizeof(array)/sizeof(array[0]);
  for (int i = 0; i <= size; i++){
    add = add + array[i];
  }
}
int main( int argc, char ** argv) {
  
  FILE * fPointer;
  char singleLine[150];
  int numbers[150];
  int index = 0;
  fPointer = fopen(argv[1], "r");

  while(!feof(fPointer)){  
    fgets(singleLine, 150, fPointer);
    numbers[index] = atoi(singleLine);
    index++;
    singleLine[0] = '\0';
  }

  fclose(fPointer);

  int i = 0;
  float numerator = (float) (index - 1);
  float denominator = (float) atoi(argv[2]);
  int groupsize = ceil(numerator/denominator);
  pthread_t tids[atoi(argv[2]) - 1];
  int n = 0;
  while(n <= atoi(argv[2]) - 1){
    int subgroup[groupsize - 1];
    if (n == atoi(argv[2]) - 1){
       groupsize = (index - 1) - i;
    }
    if (groupsize - 1 < 0) {
      subgroup[0] = 0;
    }

    int j = 0;
    while(j <= groupsize - 1){
      subgroup[j] = numbers[i + j];
      j++;
    } 
    pthread_create(&tids[n], NULL, &sum, (void*) subgroup);

    pthread_join(tids[n], NULL);
    printf("Thread %d: %d\n", n, add);
    sums[n] = add;
    add = 0;
    i = i + groupsize;
    n++;
  }
  int total = 0;
  for (int i = 0; i < atoi(argv[2]) - 1; i++){
    total = total + sums[i];
  }
  printf("Sum = %d\n", total);
}


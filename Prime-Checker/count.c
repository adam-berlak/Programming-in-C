/// counts number of primes from standard input
///
/// compile with:
///   $ gcc findPrimes.c -O2 -o count -lm
///
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>

int currInd = 0;
int count = 0;
int size;
int64_t numbers[10];
pthread_mutex_t indLock;
pthread_mutex_t resLock;

struct isPrime_sruct {
	int64_t num;
	int answer;
};

/// primality test, if n is prime, return 1, else return 0
void* isPrime(void* arg)
{
	while(1){
		 int numInd;
		 int notPrime = 0;
		 pthread_mutex_lock(&indLock);
		 if(currInd == size) {
			 pthread_mutex_unlock(&indLock);
			 break;
		 }
		printf("current index is %d\n", currInd);
		 numInd = currInd;
		 currInd++;
		 pthread_mutex_unlock(&indLock);
		 int64_t n = numbers[numInd];
		printf("the number is %lld\n", n);
		 if( n <= 1) {
			printf("if 1 called\n");
			 continue;
		 }
		 if( n <= 3) {
			printf("if 2 called\n");
				pthread_mutex_lock(&resLock);
				count++;
				printf("incrementing count, count is %d\n", count);
				pthread_mutex_unlock(&resLock);
				continue;
		 }
		 if( n % 2 == 0 || n % 3 == 0) {
			printf("if 3 called\n");
			 continue;
		 } 
		 printf("if 4 called\n");
		int64_t i = 5;
		int64_t max = sqrt(n);
		while( i <= max) {
			if (n % i == 0 || n % (i+2) == 0) {
				notPrime = 1;
				break;
			}
			i += 6;
		}
		if (notPrime == 1) {
			continue;
		}
		printf("exited while loop");
		 pthread_mutex_lock(&resLock);
		 count++;
		 pthread_mutex_unlock(&resLock);
		
	}
	pthread_exit(0);
	
}

int main( int argc, char ** argv)
{
    /// parse command line arguments
    int nThreads = 1;
    if( argc != 1 && argc != 2) {
        printf("Uasge: countPrimes [nThreads]\n");
        exit(-1);
    }
    if( argc == 2) nThreads = atoi( argv[1]);

    /// handle invalid arguments
    if( nThreads < 1 || nThreads > 256) {
        printf("Bad arguments. 1 <= nThreads <= 256!\n");
    }

    /// count the primes
    printf("Counting primes using %d thread%s.\n",
           nThreads, nThreads == 1 ? "s" : "");
    
	
	
	
	struct isPrime_sruct args[50];
	int i = 0;
	
    while(1) {
        int64_t num;
        if( 1 != scanf("%lld", & num)) break;
		numbers[i] = num;
		size++;
		i++;
    }
		
	pthread_mutex_init(&indLock, NULL);
	pthread_mutex_init(&resLock, NULL);
	pthread_t tids[nThreads];

	for(int i=0; i < nThreads; i++) {
		pthread_create(&tids[i], NULL, isPrime, NULL);
	}
	
	for (int i = 0; i < nThreads; i++) {
		pthread_join(tids[i], NULL);
	}


    /// report results
    printf("Found %d primes.\n", count);

    return 0;
}


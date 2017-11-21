#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>

#define CEILING(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))


#define BALLOT_SIZE 1503
#define MAX_DEPTH_OF_TREE 5
#define MAX_SIZE_OF_TREE ((1 << MAX_DEPTH_OF_TREE) - 1)
#define TREE_COUNT ((int) CEILING(BALLOT_SIZE/MAX_SIZE_OF_TREE))
#define NUM_VOTERS 10000
#define MAX_THREAD_COUNT 1000

//locks for asynchronous reading and writing from trees
pthread_rwlock_t * rwlocks;

//global variable for number of processors
long numProcessors;

//global variable that holds all of the votes
struct Ballot ** Ballots;

int Results[BALLOT_SIZE] = {0};

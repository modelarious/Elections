/*
Based on video: 
Voting systems and the Condorcet Paradox || infinite series youtube channel
https://www.youtube.com/watch?v=HoAnYQZrNrQ
SQL database from:
http://www.quietaffiliate.com/free-first-name-and-last-name-databases-csv-and-sql/
*/

//randomly select BALLOT_SIZE permutations of possible ballots, then generate random 
//numbers in a largish range (0-1000), one for each permutation, then divide all those 
//numbers by the total, those are the percent chances that that permutation will be chosen


#include "Build_Balanced_Tree_MULTI.c"
//#include "Settings.h"

#define SHM_PATH "/tmp/SHMEM"


//Implemented as a struct to allow for easy additions to the ballot if necessary
//such as name, or maybe some more in depth voter data if this were a more involved
//simulation

//struct internalThreadData * theadData;
/*

thread_count = ceil(thread_index_range/total_index_range);
if (MAX_THREAD_COUNT > 0) {
	thread_count = min(MAX_THREAD_COUNT, thread_count);
}
internalThreadData = malloc(thread_count);
*/
struct Ballot {
	int * votes;
};
/*
typedef enum {
	INIT,
	IN_PROGRESS,
	COMPLETE,
	ERR
} internal_thread_states_t ;
*/
/*
struct internalThreadData {
	//internal_thread_states_t internalState;
	int start_index;
	int stop_index;
	int (*functionPtr) (int strt, int stp);
};
*/
/*
struct shmem_variables {
	int plurality_winner;
	int second_winner;
	int third_winner;
	int fourth_winner;
};
*/
struct threadData {
	//internal_thread_states_t internalState;
	pthread_t tid;
	int start_index;
	int stop_index;
};

struct DefaultBallot {
	int votes[5];
};

void shuffle(int *array, size_t size) {    
    if (size > 1) {
        int currPos;
        for (currPos = size - 1; currPos > 0; currPos--) {
            int swapPos = (drand48()*(currPos+1));
            int toSwap = array[swapPos];
            array[swapPos] = array[currPos];
            array[currPos] = toSwap;
        }
    }
}

struct DefaultBallot * New_Default_Ballot(int votes[5]) {

	struct DefaultBallot * New_Ballot = malloc(sizeof(struct DefaultBallot));
	for (int i = 0; i < 5; i++) {
		New_Ballot->votes[i] = votes[i];
	}
	return New_Ballot;
}

struct Ballot * New_Random_Ballot() {

	int * v = malloc(BALLOT_SIZE*sizeof(int)); 
	if (v == NULL) {
		fprintf(stderr, "Couldn't allocate the space for votes in a New Random Ballot\n");
		exit(-1);
	}
	
	for (int i = 0; i < BALLOT_SIZE; i++) {
		v[i] = i+1;
	}
	shuffle(v, BALLOT_SIZE);
	
	struct Ballot * New_Ballot = malloc(sizeof(struct Ballot));
	
	if (New_Ballot == NULL) {
		fprintf(stderr, "Couldn't allocate enough space for a new ballot\n");
		exit(-1);
	}
	New_Ballot->votes = v;
	/*
	for (int j = 0; j < BALLOT_SIZE; j++) {
		New_Ballot->votes[j] = v[j];
	}
	*/
	return New_Ballot;
}

void Print_Results(int Results[], size_t len) {
/*
	for (int i = 0; i < len; i++) {
		printf("Candidate %d: %d\n", i+1, Results[i]);
	}
*/
}

void Print_Default_Array(struct DefaultBallot * Ballots[55]) {
/*
	for (int i = 0; i < 55; i++) {
		printf("Ballot Number %d:\n", i+1);
		for (int j = 0; j < 5; j++) {
			printf("%d ", Ballots[i]->votes[j]);
		}
		printf("\n");
	}	
*/
}

void Print_Array(struct Ballot * Ballots[NUM_VOTERS]) {
/*
	for (int i = 0; i < NUM_VOTERS; i++) {
		printf("Ballot Number %d:\n", i+1);
		for (int j = 0; j < BALLOT_SIZE; j++) {
			printf("%d ", Ballots[i]->votes[j]);
		}
		printf("\n");
	}
*/	
}

void Hold_Default_Election(struct DefaultBallot * Ballots[55]) {
	//Dataset used in the video
	int count = 0;
	int vote1[5] = {1,5,4,2,3}; //18
	int vote2[5] = {5,1,4,3,2}; //12
	int vote3[5] = {5,2,1,4,3}; //10
	int vote4[5] = {5,4,2,1,3}; //9
	int vote5[5] = {5,2,4,3,1}; //4
	int vote6[5] = {5,4,2,3,1}; //2
	
	for (; count < 18; count++) {
		Ballots[count] = New_Default_Ballot(vote1);
	}
	
	for (; count < 18+12; count++) {
		Ballots[count] = New_Default_Ballot(vote2);
	}
	
	for (; count < 18+12+10; count++) {
		Ballots[count] = New_Default_Ballot(vote3);
	}
	
	for (; count < 18+12+10+9; count++) {
		Ballots[count] = New_Default_Ballot(vote4);
	}
	
	for (; count < 18+12+10+9+4; count++) {
		Ballots[count] = New_Default_Ballot(vote5);
	}
	
	for (; count < 18+12+10+9+4+2; count++) {
		Ballots[count] = New_Default_Ballot(vote6);
	}
}

void Hold_Election() {

	//get time of day in usec and use it to seed random number generator
	struct timeval time;
    gettimeofday(&time, NULL);
    int usec = time.tv_usec;
    srand48(usec);

    //cast votes
	for (int i=0; i < NUM_VOTERS; i++) {
		for (int j = 0; j < BALLOT_SIZE; j++) {
			Ballots[i]->votes[j] = j+1;
		}
		shuffle(Ballots[i]->votes, BALLOT_SIZE);
	}
	
}

void * cast_votes(void * arg) {
	struct threadData * vote_info = (struct threadData * ) arg;
	
	//cast votes
	for (int i=vote_info->start_index; i < vote_info->stop_index; i++) {
		for (int j = 0; j < BALLOT_SIZE; j++) {
			Ballots[i]->votes[j] = j+1;
		}
		shuffle(Ballots[i]->votes, BALLOT_SIZE);
		
		//printf("Ballots[%d] contents: %d %d\n", i, Ballots[i]->votes[0], Ballots[i]->votes[1]);
		
	}
	
	return 0;
}
	
	

void Hold_Election_Multi() {

	int rc;

	//get time of day in usec and use it to seed random number generator
	struct timeval time;
    gettimeofday(&time, NULL);
    int usec = time.tv_usec;
    srand48(usec);
    
    struct threadData * data;
    data = calloc(numProcessors, sizeof(struct threadData));
    
    int interval = ceil(NUM_VOTERS/numProcessors);
    int start=0;
    int stop=interval;
    
    for (int i=0; i < numProcessors; i++) {
    	
    	if (stop >= NUM_VOTERS) {
    		stop = NUM_VOTERS-1;
    	}
    	
    	assert(start < NUM_VOTERS-1);
    	
    	data[i].start_index = start;
    	data[i].stop_index = stop;
    	
    	rc = pthread_create(&data[i].tid, NULL, cast_votes, &data[i]);
    	if (rc != 0) {
    		fprintf(stderr, "Error in pthread_create: %d %s\n", errno, strerror(errno));
    		exit(-1);
    	}
    	start += interval;
    	stop += interval;
    }
    
    for (int i=0; i < numProcessors; i++) {
    	pthread_join(data[i].tid, NULL);
    }
	
}

int Argmax(int Results[], size_t length) {
	int finalResult = -2;
	int maxResult = 0;
	for (int i = 0; i < length; i++) {
		if (Results[i] > maxResult) {
			finalResult = i;
			maxResult = Results[i];
		}
	}
	return finalResult + 1;
}

void Tally_Results(struct Ballot * Ballots[NUM_VOTERS], int Results[BALLOT_SIZE]) {
	for (int i = 0; i < NUM_VOTERS; i++) {
		for (int j = 0; j < BALLOT_SIZE; j++) {
			if (Ballots[i]->votes[j] == 1) {
				Results[j]++;
			}
		}
	}
}
	
int Plurality(struct Ballot * Ballots[NUM_VOTERS]) {
	printf("Plurality:\n");
	int Results[BALLOT_SIZE] = {0};
	Tally_Results(Ballots, Results);
	Print_Results(Results, BALLOT_SIZE);
	int finalResult = Argmax(Results, BALLOT_SIZE);
	if (finalResult <= 0) {
		perror("Ballots array was blank, Plurality winner couldn't be determined\n");
		return -1;
	}
	printf("%d is the Plurality winner\n\n", finalResult);
	return finalResult;
}

void * Tally_Results_Multi(void * arg) {
	struct threadData * vote_info = (struct threadData * ) arg;
	
	//cast votes
	for (int i=vote_info->start_index; i < vote_info->stop_index; i++) {
		for (int j = 0; j < BALLOT_SIZE; j++) {
			if (Ballots[i]->votes[j] == 1) {
				Results[j]++;
			}
		}
	}
	
	return 0;
}

int Plurality_Multi() {
	printf("Plurality Multi:\n");
	int rc;
	
	struct threadData * data;
    data = calloc(numProcessors, sizeof(struct threadData));
	int interval = ceil(NUM_VOTERS/numProcessors);
    int start=0;
    int stop=interval;
    
    for (int i=0; i < numProcessors; i++) {
    	
    	if (stop >= NUM_VOTERS) {
    		stop = NUM_VOTERS-1;
    	}
    	
    	assert(start < NUM_VOTERS-1);
    	
    	data[i].start_index = start;
    	data[i].stop_index = stop;
    	
    	//Tally_Results_Multi(Results, start, stop);
    	
    	rc = pthread_create(&data[i].tid, NULL, Tally_Results_Multi, &data[i]);
    	if (rc != 0) {
    		fprintf(stderr, "Error in pthread_create: %d %s\n", errno, strerror(errno));
    		exit(-1);
    	}
    	start += interval;
    	stop += interval;
    }
    
    for (int i=0; i < numProcessors; i++) {
    	pthread_join(data[i].tid, NULL);
    }
    
	int finalResult = Argmax(Results, BALLOT_SIZE);
	if (finalResult <= 0) {
		perror("Ballots array was blank, Plurality winner couldn't be determined\n");
		return -1;
	}
	printf("%d is the Plurality winner\n\n", finalResult);
	return finalResult;
}

void Top_Two(int Results[BALLOT_SIZE], int * Biggest, int * BiggestPos, int * secondBiggest, int * secondBiggestPos) {
	//find top two candidates
	
	for (int i = 0; i < BALLOT_SIZE; i++) {
		
		if (Results[i] > *Biggest) {
			*secondBiggest = *Biggest;
			*secondBiggestPos = *BiggestPos;
			*Biggest = Results[i];
			*BiggestPos = i;
		}
		else if (Results[i] > *secondBiggest) {
			*secondBiggest = Results[i];
			*secondBiggestPos = i;
		}
	}
}

void Tally_Secondary_Results(struct Ballot * Ballots[NUM_VOTERS], int SecondResults[2], 
							int CandidateOne, int CandidateTwo, int BiggestPos, 
							int secondBiggestPos) {
							
	for (int i = 0; i < NUM_VOTERS; i++) {
		
		//see if vote at BiggestPos is smaller than vote at secondBiggestPos (indicates higher preference)
		if (Ballots[i]->votes[BiggestPos] < Ballots[i]->votes[secondBiggestPos]) {
			SecondResults[CandidateOne]++;
		}
		else {
			SecondResults[CandidateTwo]++;
		}
	}
}

int Two_Round_Runoff(struct Ballot * Ballots[NUM_VOTERS]) {
	printf("Two Round Runoff:\n");
	int Results[BALLOT_SIZE] ={0};
	Tally_Results(Ballots, Results);
	Print_Results(Results, BALLOT_SIZE);
	int Biggest = -1, BiggestPos = -1, secondBiggest = -1, secondBiggestPos = -1;

	Top_Two(Results, &Biggest, &BiggestPos, &secondBiggest, &secondBiggestPos);
	
	//if we have a majority vote right away, that's the winner
	float top = Biggest/NUM_VOTERS;
	if (top >= 0.5) {
		printf("%d is the Two Round Runoff winner with %f percent of the votes.\n", BiggestPos+1, top);
		return BiggestPos+1;
	}

	int CandidateOne = 0;
	int CandidateTwo = 1;
	int SecondResults[2] = {0};
	
	//printf("Secondary results with Candidate %d now as Candidate 1 and Candidate %d now as Candidate 2:\n", BiggestPos+1, secondBiggestPos+1);
	
	Tally_Secondary_Results(Ballots, SecondResults, CandidateOne, CandidateTwo, BiggestPos, secondBiggestPos);
	Print_Results(SecondResults, 2);
	
	if (SecondResults[CandidateOne] >= SecondResults[CandidateTwo] ) {
		printf("%d is the Two Round Runoff winner\n", BiggestPos+1);
		return BiggestPos+1;
	}
	else {
		printf("%d is the Two Round Runoff winner\n", secondBiggestPos+1);
		return secondBiggestPos+1;
	}
}

int Borda_Count(struct Ballot ** Ballots) {
	printf("\nBorda Count:\n");
	int *bordaTotals = calloc(BALLOT_SIZE, sizeof(int));
	
	if (bordaTotals == NULL) {
		fprintf(stderr, "Unable to allocate enough memory for borda totals\n");
		exit(-1);
	}
	
	for (int i = 0; i < NUM_VOTERS; i++) {
		for (int j = 0; j < BALLOT_SIZE; j++) {
			bordaTotals[j] = bordaTotals[j] + (BALLOT_SIZE + 1) - Ballots[i]->votes[j];
		}
	}
	
	printf("%d is the Borda Count winner\n", Argmax(bordaTotals, BALLOT_SIZE));
	free(bordaTotals);
	return 0;
}

int Default_Plurality(struct DefaultBallot * Ballots[55]) {
	int Results[5] ={0};
	int finalResult = -1; // will be changed based on who has the most votes in the end
	
	for (int i = 0; i < 55; i++) {
		for (int j = 0; j < 5; j++) {
			if (Ballots[i]->votes[j] == 1) {
				Results[j]++;
			}
		}
	}
	
	int maxResult = 0;
	for (int i = 0; i < 5; i++) {
		if (Results[i] > maxResult) {
			finalResult = i;
			maxResult = Results[i];
		}
	}
	finalResult++;
	printf("%d is the Plurality winner\n", finalResult);
	return finalResult;
}
		
int Default_Two_Round_Runoff(struct DefaultBallot * Ballots[55]) {
	int Results[5] ={0};
	
	//tally up results
	for (int i = 0; i < 55; i++) {
		for (int j = 0; j < 5; j++) {
			if (Ballots[i]->votes[j] == 1) {
				Results[j]++;
			}
		}
	}
	
	//find top two candidates
	int Biggest = -1;
	int BiggestPos = -1;
	int secondBiggest = -1;
	int secondBiggestPos = -1;
	
	for (int i = 0; i < 5; i++) {
		if (Results[i] > Biggest) {
			secondBiggest = Biggest;
			secondBiggestPos = BiggestPos;
			Biggest = Results[i];
			BiggestPos = i;
		}
		else if (Results[i] > secondBiggest) {
			secondBiggest = Results[i];
			secondBiggestPos = i;
		}
	}
		
	//if we have a majority vote right away, that's the winner
	float top = Biggest/55;
	if (top >= 0.5) {
		printf("%d is the Two Round Runoff winner with %f percent of the votes.\n", BiggestPos+1, top);
		return BiggestPos+1;
	}
	
	int CandidateOne = 0;
	int CandidateTwo = 1;
	int SecondResults[2] = {0};
	
	for (int i = 0; i < 55; i++) {
		
		//see if vote at BiggestPos is smaller than vote at secondBiggestPos (indicates higher preference)
		if (Ballots[i]->votes[BiggestPos] < Ballots[i]->votes[secondBiggestPos]) {
			SecondResults[CandidateOne]++;
		}
		else {
			SecondResults[CandidateTwo]++;
		}
			
	}
		
	if (SecondResults[CandidateOne] >= SecondResults[CandidateTwo] ) {
		printf("%d is the Two Round Runoff winner\n", BiggestPos+1);
		return BiggestPos+1;
	}
	else {
		printf("%d is the Two Round Runoff winner\n", secondBiggestPos+1);
		return secondBiggestPos+1;
	}
}

//int Default_Instant_Runoff(struct DefaultBallot * Ballots[55]) {
	
void Default_Dot_Product(int a[5][5], int output[5]) {

	for (int i = 0; i < 5; i++) {
		int total = 0;
		for (int j = 0; j < 5; j++) {
			total = total + a[i][j] * (5 - (j+1));
		}
		output[i] = total;
	}
}

int Default_Borda_Count(struct DefaultBallot * Ballots[55]) {

	//rows are candidates
	//columns are how many times they received each ranking
	//for example, if there are 3 candidates and 5 voters we might get this Pseudocode:
	// Results[3][3] = [[0, 0, 5],
	//                  [2, 3, 0],
	//                  [3, 2, 0]]
	//which indicates that: 
	//Candidate 1 received ranking 3 5 times
	//Candidate 2 received ranking 1 twice and 2 thrice
	//Candidate 3 received ranking 1 thrice and 2 twice
	
	int Results[5][5] ={{0}};
	for (int i = 0; i < 55; i++) {
		for (int j = 0; j < 5; j++) {
			Results[j][Ballots[i]->votes[j]-1]++;
		}
	}
	
	int DotResult[5];
	
	Default_Dot_Product(Results, DotResult);
	
	//Print_Results(DotResult, 5);
	
	int finalResult = -1;
	int maxResult = 0;
	for (int i = 0; i < 5; i++) {
		if (DotResult[i] > maxResult) {
			finalResult = i;
			maxResult = DotResult[i];
		}
	}
	finalResult++;
	
	printf("%d is the Borda Count winner.\n", finalResult);
	return finalResult;
}

int func() {
	int numTrees = (int)ceil((float)BALLOT_SIZE/MAX_SIZE_OF_TREE);
	int * Counts = malloc(numTrees * sizeof(int));
	struct Node ** Containers = run(Counts, numTrees);
	printf("%d\n", Retrieve(Containers[which(5)], 5)->value);
	printf("%d\n", Retrieve(Containers[which(101)], 101)->value);
	printf("%d\n", Counts[0]);
	
	int bin;
	
	int * treeContents = malloc(BALLOT_SIZE*sizeof(int));
	full_In_Order_Traversal(treeContents, Counts, numTrees, Containers);
	for (int j = 0; j < BALLOT_SIZE; j++) {
		//printf("\nDelete %d\n", j);
		bin = which(j);
		if (Delete(Containers[bin], j, Counts) == 0) {
			//assert(Counts[bin] >= 0);
		}
		full_In_Order_Traversal(treeContents, Counts, numTrees, Containers);
	}
	
	for (int i = 0; i < numTrees; i++) {
		if (Counts[i] != 0) {
			fprintf(stderr, "Counts[%d] was not 0\n", i);
			exit(-1);
		}
	}
	
	
	for (int i = 0; i < numTrees; i++) {
		Deforestation(Containers[i]);
	}
	
	return 0;
}


void * thread_func(void * arg) {
/*
	struct internalThreadData *ITD = (struct internalThreadData*)arg;

	kill_me:
		//Signal the main thead that something went wrong
		//ITD->internalState = ERR;
		return 0;
*/
	return 0;
}
		

int Synchronization_Init() {
	int fd;
	char shmem_path[100];
	int rc;
	
	sprintf(shmem_path, "%s_%d", SHM_PATH, getpid());
	
	fd = shm_open(shmem_path, O_CREAT | O_RDWR, 0777);
	if( fd == -1 ) {
        fprintf(stderr, "Failure on shm_open call: errNum %d: (%s)\n", errno, strerror(errno));
        return -1;
    }
    
    if(ftruncate(fd, sizeof(pthread_rwlock_t) * TREE_COUNT) == -1) {
        fprintf(stderr, "Failure on ftruncate call: errNum %d: (%s)\n", errno, strerror(errno));
        return -1;
    }

    rwlocks = mmap(0, sizeof(pthread_rwlock_t) * TREE_COUNT, PROT_WRITE| PROT_READ, MAP_SHARED, fd, 0);
    if(rwlocks == MAP_FAILED) {
        fprintf(stderr, "Failure on mmap call: errNum %d: (%s)\n", errno, strerror(errno));
        return -1;
    }
	
	//create attribute object and make it shared across process spaces
	pthread_rwlockattr_t attr;
	rc = pthread_rwlockattr_init(&attr);
	if (rc != 0) {
		fprintf(stderr, "Failure on pthread_rwlockattr_init. %d (%s)\n", rc, strerror(rc));
		return -1;
	}
	
	rc = pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	if (rc != 0) {
		fprintf(stderr, "Failure on pthread_rwlockattr_setpshared. %d (%s)\n", rc, strerror(rc));
		return -1;
	}
	
	//allocate space for the rwlocks and initialize them with attr
	rwlocks = malloc(sizeof(pthread_rwlock_t) * TREE_COUNT);
	if (rwlocks == NULL) {
		fprintf(stderr, "Failure on malloc of rwlocks. %d (%s)\n", errno, strerror(errno));
		return -1;
	}
	
	for (int index = 0; index < TREE_COUNT; index++) {
		rc = pthread_rwlock_init(&rwlocks[index], &attr);
		if (rc != 0) {
			fprintf(stderr, "Failure on pthread_rwlock_init index %d. %d (%s)\n", index, rc, strerror(rc));
			return -1;
		}
	}
	return 0;
}

	

int main(int argc, char **argv) {
    
    //Initialize the memory outside of the thread functions
	Ballots = malloc(NUM_VOTERS*sizeof(struct Ballot*));
	if (Ballots == NULL) {
		fprintf(stderr, "error on malloc call for Ballots. %d (%s)\n", errno, strerror(errno));
		exit(-1);
	}
	
	for (int i = 0; i < NUM_VOTERS; i++) {
		Ballots[i] = malloc(BALLOT_SIZE*sizeof(struct Ballot));
		if (Ballots[i] == NULL) {
			fprintf(stderr, "error on malloc call for Ballots. %d (%s)\n", errno, strerror(errno));
			exit(-1);
		}

		Ballots[i]->votes = calloc(BALLOT_SIZE, sizeof(int));
		if (Ballots[i]->votes == NULL) {
			fprintf(stderr, "Couldn't allocate the space for votes in a New Random Ballot\n");
			exit(-1);
		}
	}


	numProcessors = sysconf(_SC_NPROCESSORS_ONLN);
	if (numProcessors == 1) {
		/* Run single threaded version */
		printf("Only one processor?\n");
		Hold_Election();
	}
	else {
	    /* Run Multi threaded version */
	    printf("I've detected %ld processors\n", numProcessors);
	    Synchronization_Init();
	    Hold_Election_Multi();
	    Plurality_Multi();
	}
	
	Plurality(Ballots);
	Two_Round_Runoff(Ballots);
	Borda_Count(Ballots);

	//free memory
	
	for (int i=0; i < NUM_VOTERS; i++) {
		free(Ballots[i]);
	}
	free(Ballots);
	
	
	printf("\nDefaults\n");
	
	struct DefaultBallot * Default_Ballots[55];
	Hold_Default_Election(Default_Ballots);
	//Print_Default_Array(Default_Ballots);
	Default_Plurality(Default_Ballots);
	Default_Two_Round_Runoff(Default_Ballots);

	Default_Borda_Count(Default_Ballots);
	func();
	
	for (int i=0; i < 55; i++) {
		free(Default_Ballots[i]);
	}
	
	
	
	
	return 0;
}

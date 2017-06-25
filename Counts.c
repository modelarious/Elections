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

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#define BALLOT_SIZE 12
#define NUM_VOTERS 560

//Implemented as a struct to allow for easy additions to the ballot if necessary
//such as name, or maybe some more in depth voter data if this were a more involved
//simulation

struct Ballot {
	int votes[BALLOT_SIZE];
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

	int v[BALLOT_SIZE]; 
	for (int i = 0; i < BALLOT_SIZE; i++) {
		v[i] = i+1;
	}
	shuffle(v, BALLOT_SIZE);
	
	struct Ballot * New_Ballot = malloc(sizeof(struct Ballot));
	for (int j = 0; j < BALLOT_SIZE; j++) {
		New_Ballot->votes[j] = v[j];
	}
	
	return New_Ballot;
}

void Print_Results(int Results[], size_t len) {
	for (int i = 0; i < len; i++) {
		printf("Candidate %d: %d\n", i+1, Results[i]);
	}
	
}

void Print_Default_Array(struct DefaultBallot * Ballots[55]) {
	
	for (int i = 0; i < 55; i++) {
		printf("Ballot Number %d:\n", i+1);
		for (int j = 0; j < 5; j++) {
			printf("%d ", Ballots[i]->votes[j]);
		}
		printf("\n");
	}	
}

void Print_Array(struct Ballot * Ballots[NUM_VOTERS]) {
	
	for (int i = 0; i < NUM_VOTERS; i++) {
		printf("Ballot Number %d:\n", i+1);
		for (int j = 0; j < BALLOT_SIZE; j++) {
			printf("%d ", Ballots[i]->votes[j]);
		}
		printf("\n");
	}	
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

void Hold_Election(struct Ballot * Ballots[NUM_VOTERS]) {

	//get time of day in usec and use it to seed random number generator
	struct timeval time;
    gettimeofday(&time, NULL);
    int usec = time.tv_usec;
    srand48(usec);

    //cast votes
	for (int i=0; i < NUM_VOTERS; i++) {
		Ballots[i] = New_Random_Ballot();
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
	
	printf("Secondary results with Candidate %d now as Candidate 1 and Candidate %d now as Candidate 2:\n", BiggestPos+1, secondBiggestPos+1);
	
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

void Dot_Product(int a[BALLOT_SIZE][BALLOT_SIZE], int output[BALLOT_SIZE]) {

	for (int i = 0; i < BALLOT_SIZE; i++) {
		int total = 0;
		for (int j = 0; j < BALLOT_SIZE; j++) {
			total = total + a[i][j] * (BALLOT_SIZE - (j+1));
		}
		output[i] = total;
	}
}

void Tally_Borda(int Results[BALLOT_SIZE][BALLOT_SIZE], struct Ballot * Ballots[NUM_VOTERS]) {
	for (int i = 0; i < NUM_VOTERS; i++) {
		for (int j = 0; j < BALLOT_SIZE; j++) {
			Results[j][Ballots[i]->votes[j]-1]++;
		}
	}
}
	
int Borda_Count(struct Ballot * Ballots[NUM_VOTERS]) {

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
	
	printf("\nBorda Count:\n");
	int Results[BALLOT_SIZE][BALLOT_SIZE] ={{0}};
	Tally_Borda(Results, Ballots);
	
	int DotResult[BALLOT_SIZE];
	Dot_Product(Results, DotResult);
	Print_Results(DotResult, BALLOT_SIZE);

	int finalResult = Argmax(DotResult, BALLOT_SIZE);
	printf("%d is the Borda Count winner\n", finalResult);
	return finalResult;
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
	// Results[3][3] = [[0, 0, 3],
	//                  [1, 2, 0],
	//                  [2, 1, 0]]
	//which indicates that: 
	//Candidate 1 received ranking 3, 3 times
	//Candidate 2 received ranking 1 once and 2 twice
	//Candidate 3 received ranking 1 twice and 2 once
	
	int Results[5][5] ={{0}};
	for (int i = 0; i < 55; i++) {
		for (int j = 0; j < 5; j++) {
			Results[j][Ballots[i]->votes[j]-1]++;
		}
	}
	
	int DotResult[5];
	
	Default_Dot_Product(Results, DotResult);
	
	Print_Results(DotResult, 5);
	
	int finalResult = -1;
	int maxResult = 0;
	for (int i = 0; i < 5; i++) {
		if (DotResult[i] > maxResult) {
			finalResult = i;
			maxResult = DotResult[i];
		}
	}
	finalResult++;
	
	printf("Borda Count winner is %d.\n", finalResult);
	return finalResult;
}


int main(int argc, char **argv) {
	struct Ballot * Ballots[NUM_VOTERS];
	Hold_Election(Ballots);
	//Print_Array(Ballots);
	Plurality(Ballots);
	Two_Round_Runoff(Ballots);
	Borda_Count(Ballots);
	//free memory
	for (int i=0; i < NUM_VOTERS; i++) {
		free(Ballots[i]);
	}
	
	printf("\nDefaults\n");
	
	struct DefaultBallot * Default_Ballots[55];
	Hold_Default_Election(Default_Ballots);
	//Print_Default_Array(Default_Ballots);
	Default_Plurality(Default_Ballots);
	Default_Two_Round_Runoff(Default_Ballots);
	
	Default_Borda_Count(Default_Ballots);
	
	for (int i=0; i < 55; i++) {
		free(Default_Ballots[i]);
	}
	return 0;
}

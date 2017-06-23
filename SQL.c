#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sqlite3.h>
#define BALLOT_SIZE 30
#define NUM_VOTERS 100

int Min(int a, int b) {
	if (a<=b) return a;
	return b;
}

void Double_Shuffle(int array[], char *array2[], size_t size) {    
    if (size > 1) {
        int currPos;
        for (currPos = size - 1; currPos > 0; currPos--) {
            int swapPos = (drand48()*(currPos+1));
            int toSwap = array[swapPos];
            char * toSwap2 = array2[swapPos];
            array[swapPos] = array[currPos];
            array2[swapPos] = array2[currPos];
            array[currPos] = toSwap;
            array2[currPos] = toSwap2;
        }
    }
}

long int Query_Max_Number_Of_Names(sqlite3 *db, sqlite3_stmt *stmt) {
	
	int rv;
	
	char * sql_stmt = "select min(t1.val, t2.val) from " 
					  "(select count(*) as val from firstnames) as t1, " 
					  "(select count(*) as val from lastnames) as t2;";
					  
	rv = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);
	
	if (rv != SQLITE_OK) {
		fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	
	int result = -1;
	if ((rv = sqlite3_step(stmt)) == SQLITE_ROW) {
		result = strtol((const char *)sqlite3_column_text(stmt, 0), (char**) NULL, 10);
	}
	
	return result;
	
}

int Get_Sub_Names(sqlite3 *db, sqlite3_stmt *stmt, char * sql_stmt, 
					char * names[], int * lengths, size_t maxLength) {
	int rv;
	rv = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);
	
	if (rv != SQLITE_OK) {
		fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	sqlite3_bind_int(stmt, 1, maxLength);
	
	int counter = 0;
	while ((rv = sqlite3_step(stmt)) == SQLITE_ROW) {
		lengths[counter] = strtol((const char *)sqlite3_column_text(stmt, 0), (char**) NULL, 10);
		names[counter++] = strdup((const char *)sqlite3_column_text(stmt, 1));
	}
	
	return 0;
}


int Get_Full_Names(sqlite3 *db, sqlite3_stmt *stmt, char * fullnames[], size_t maxLength) {
	int rv;
	char * firstnames[maxLength];
	int firstnameLengths[maxLength];
	char * lastnames[maxLength];
	int lastnameLengths[maxLength];
	
	char *sql_stmt = "select length(firstname), firstname from firstnames ORDER BY RANDOM() LIMIT ?;";
	char *sql_stmt2 = "select length(lastname), lastname from lastnames ORDER BY RANDOM() LIMIT ?;";
	
	rv = Get_Sub_Names(db, stmt, sql_stmt, firstnames, firstnameLengths, maxLength);

	if (rv == -1) {
		fprintf(stderr, "Failed to get firstnames\n");
		return -1;
	}
	
	rv = Get_Sub_Names(db, stmt, sql_stmt2, lastnames, lastnameLengths, maxLength);
	
	if (rv == -1) {
		fprintf(stderr, "Failed to get lastnames\n");
		return -1;
	}
	
	for (int currSet = 0; currSet < BALLOT_SIZE; currSet=currSet+maxLength) {
			
			//when we get close to the top, need to make sure we don't make too many names
			int currSize = Min(BALLOT_SIZE - currSet, maxLength);
			
			if (currSet != 0) {
				Double_Shuffle(firstnameLengths, firstnames, maxLength);
				Double_Shuffle(lastnameLengths, lastnames, maxLength);
			}
			
			for (int i = 0; i < currSize; i++) {

				//+ 2 because we are also adding a space between the words
				fullnames[i+currSet] = malloc(firstnameLengths[i] + lastnameLengths[i] + 2);
				sprintf(fullnames[i+currSet], "%s %s", firstnames[i], lastnames[i]);
			}
	}
	
	for (int i = 0; i < maxLength; i++) {
		free(firstnames[i]);
		free(lastnames[i]);
	}

	return 0;
}


int main() {
	sqlite3 *db;
	sqlite3_stmt *stmt = NULL;
	
	int rv;
	
	rv = sqlite3_open("NamesDatabase.db", &db);
	
	if (rv) {
		fprintf(stderr, "Failed to open database %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	
	int maxSize = Query_Max_Number_Of_Names(db, stmt);
	
	if (maxSize <= 0) {
		fprintf(stderr, "Database contains no names!\n");
		sqlite3_close(db);
		return -1;
	}
	
	char ** fullnames = calloc(BALLOT_SIZE, 26);
	
	if (BALLOT_SIZE > maxSize) {
		rv = Get_Full_Names(db, stmt, fullnames, maxSize);
	}
	else {
		rv = Get_Full_Names(db, stmt, fullnames, BALLOT_SIZE);
	}
		
	if (rv == -1) {
		fprintf(stderr, "Failed to get names\n");
		sqlite3_close(db);
		return -1;
	}
	
	for (int i = 0; i < BALLOT_SIZE; i++) {
		printf("%s\n", fullnames[i]);
	}
	
	
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	
	for (int i = 0; i < BALLOT_SIZE; i++) {
		free(fullnames[i]);
	}
	
	return 0;

}

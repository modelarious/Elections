all: SQL Counts Partial_RB_Tree Balanced

SQL: SQL.c
	gcc -g SQL.c -o GenerateNames -lsqlite3 -ldl -lpthread -std=c99 -Wall -pedantic-errors

Counts: Counts.c
	gcc Counts.c -o Counts -std=c99 -Wall -pedantic-errors
	
Partial_RB_Tree: Partial_RB_Tree.c
	gcc Partial_RB_Tree.c -o RB_Tree -std=c99 -Wall -pedantic-errors
	
Balanced: Build_Balanced_Tree.c
	gcc Build_Balanced_Tree.c -o BuildBST -std=c99 -Wall -pedantic-errors
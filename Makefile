all: SQL Counts Partial_RB_Tree

SQL: SQL.c
	gcc -g SQL.c -o GenerateNames -lsqlite3 -ldl -lpthread -std=c99 -Wall -pedantic-errors

Counts: Counts.c
	gcc Counts.c -o Counts -std=c99 -Wall -pedantic-errors
	
Partial_RB_Tree: Partial_RB_Tree.c
	gcc Partial_RB_Tree.c -o RB_Tree -std=c99 -Wall -pedantic-errors
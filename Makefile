all: SQL Counts

SQL: SQL.c
	gcc -g SQL.c -o GenerateNames -lsqlite3 -ldl -lpthread -std=c99 -Wall -pedantic-errors

Counts: Counts.c
	gcc Counts.c -o Counts -std=c99 -Wall -pedantic-errors
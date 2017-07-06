# Elections
Based on a video from Infinite Series called Voting systems and the Condorcet Paradox
https://www.youtube.com/watch?v=HoAnYQZrNrQ

Demonstrates the differences between multiple approaches to trying to determine the
winner of an election when the ballots are ranked preferences.

compile with make

run resulting files with ./GenerateNames, ./Counts and ./RB_Tree

Modifying the value of BALLOT_SIZE will change the number of candidates available for selection.
Modifying the value of and NUM_VOTERS will change the number of ballots that are cast

Currently are three separate programs, but once I get Counts completed, 
I'm going to integrate GenerateNames to create names for the candidates and use
the Partial red black tree as a way to use tree properties to my advantage for Instant Runoff.

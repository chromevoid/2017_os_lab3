# 2017_os_lab2

---------------
Compile
---------------
Make sure that main.cpp and banker.h are in the same directory. Then, compile the file:
	g++ -std=c++0x main.cpp -o banker

---------------
Run
---------------
Make sure that banker and input files are in the same directory. Then, run:
	./banker filename
The program will print the result of each algorithm (FIFO and Banker).

---------------
Sample input
---------------
2 1 4
initiate  1 1 4
request   1 1 1
release   1 1 1
terminate 1 0 0
initiate  2 1 4
request   2 1 1
release   2 1 1
terminate 2 0 0

---------------
Sample output
---------------
FIFO
Task 1    3     0     0%
Task 2    3     0     0%
total     6     0     0%

Banker
Task 1    3     0     0%
Task 2    5     2     40%
total     8     2     25%
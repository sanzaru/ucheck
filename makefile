all:
	cc -o checker checker.c csvlib.c
	
csvtest:
	cc -o csvTest csvTest.c csvlib.c

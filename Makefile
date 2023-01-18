output: golf.o
	g++ golf.o 

golf.o: golf.cpp golf.h
	g++ -c golf.cpp
	

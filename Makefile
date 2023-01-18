.PHONY: clean

golf: golf.o
	g++ golf.o -o golf 

golf.o: golf.cpp golf.h
	g++ -c golf.cpp

clean:
	-rm *.o golf	

.PHONY: clean

golf: golf.o parser.o lex.o
	g++ golf.o parser.o lex.o -o golf

golf.o: golf.cpp golf.h
	g++ -c golf.cpp

parser.o: parser.cpp parser.h
	g++ -c parser.cpp

lex.o: lex.cpp lex.h
	g++ -c lex.cpp

clean:
	-rm *.o golf	

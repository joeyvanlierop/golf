.PHONY: clean

golf: golf.o lexer.o parser.o
	g++ golf.o lexer.o parser.o -o golf

golf.o: golf.cpp golf.h
	g++ -c golf.cpp

lexer.o: lexer.cpp lexer.h
	g++ -c lexer.cpp

parser.o: parser.cpp parser.h lexer.o
	g++ -c parser.cpp

clean:
	-rm *.o golf	

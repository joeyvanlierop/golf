.PHONY: clean

golf: golf.o lexer.o parser.o
	g++ golf.o lexer.o parser.o token.o -o golf

golf.o: golf.cpp golf.h
	g++ -c golf.cpp

parser.o: parser.cpp parser.h lexer.o
	g++ -c parser.cpp

lexer.o: lexer.cpp lexer.h token.o
	g++ -c lexer.cpp

token.o: token.cpp token.h
	g++ -c token.cpp

clean:
	-rm *.o golf	

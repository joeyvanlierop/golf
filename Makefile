.PHONY: clean

golf: golf.o parser.o lex.o
	g++ golf.o parser.o lex.o -o golf

golf.o: golf.cpp golf.h
	g++ -c golf.cpp

parser.o: parser.cpp parser.h
	g++ -c parser.cpp

lexer.o: lexer.cpp lexer.h
	g++ -c lexer.cpp

clean:
	-rm *.o golf	

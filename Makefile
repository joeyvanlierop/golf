.PHONY: clean

golf: golf.o lexer.o token.o
	g++ -g golf.o lexer.o token.o -o golf

golf.o: golf.cpp golf.h
	g++ -c golf.cpp

lexer.o: lexer.cpp lexer.h token.o
	g++ -c lexer.cpp

token.o: token.cpp token.h
	g++ -c token.cpp

clean:
	-rm *.o golf.exe

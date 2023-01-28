.PHONY: clean

golf: golf.o lexer.o token.o logger.o
	g++ -g golf.o lexer.o token.o logger.o -o golf

golf.o: src/golf.cpp src/golf.h
	g++ -c golf.cpp

lexer.o: src/lexer.cpp src/lexer.h
	g++ -c lexer.cpp

token.o: src/token.cpp src/token.h
	g++ -c token.cpp

logger.o: src/logger.cpp src/logger.h
	g++ -c logger.cpp

clean:
	-rm *.o golf.exe

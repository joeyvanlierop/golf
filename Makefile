.PHONY: clean

golf: golf.o lexer.o token.o logger.o filereader.o ast.o
	g++ -g golf.o lexer.o token.o logger.o filereader.o ast.o -o golf

golf.o: src/golf.cpp src/golf.h
	g++ -c src/golf.cpp

lexer.o: src/lexer.cpp src/lexer.h
	g++ -c src/lexer.cpp

token.o: src/token.cpp src/token.h
	g++ -c src/token.cpp

logger.o: src/logger.cpp src/logger.h
	g++ -c src/logger.cpp

filereader.o: src/filereader.cpp src/filereader.h
	g++ -c src/filereader.cpp

ast.o: src/ast.cpp src/ast.h
	g++ -c src/ast.cpp

clean:
	-rm *.o golf

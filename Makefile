.PHONY: clean

golf: obj/golf.o obj/lexer.o obj/token.o obj/logger.o
	g++ -g obj/golf.o obj/lexer.o obj/token.o obj/logger.o -o golf

obj/golf.o: src/golf.cpp src/golf.h
	g++ -c golf.cpp

obj/lexer.o: src/lexer.cpp src/lexer.h
	g++ -c lexer.cpp

obj/token.o: src/token.cpp src/token.h
	g++ -c token.cpp

obj/logger.o: src/logger.cpp src/logger.h
	g++ -c logger.cpp

clean:
	-rm *.o golf.exe

.PHONY: clean

golf: golf.o lexer.o token.o logger.o input.o file_input.o repl_input.o ast.o parser.o semantic.o symbol_table.o record.o code_gen.o
	g++ -g golf.o lexer.o token.o logger.o input.o file_input.o repl_input.o ast.o parser.o semantic.o symbol_table.o record.o code_gen.o -o golf

golf.o: src/golf.cpp src/golf.h
	g++ -c src/golf.cpp

lexer.o: src/lexer.cpp src/lexer.h
	g++ -c src/lexer.cpp

token.o: src/token.cpp src/token.h
	g++ -c src/token.cpp

logger.o: src/logger.cpp src/logger.h
	g++ -c src/logger.cpp

input.o: src/input.cpp src/input.h
	g++ -c src/input.cpp

file_input.o: src/file_input.cpp src/file_input.h
	g++ -c src/file_input.cpp

repl_input.o: src/repl_input.cpp src/repl_input.h
	g++ -c src/repl_input.cpp

ast.o: src/ast.cpp src/ast.h
	g++ -c src/ast.cpp

parser.o: src/parser.cpp src/parser.h
	g++ -c src/parser.cpp

semantic.o: src/semantic.cpp src/semantic.h
	g++ -c src/semantic.cpp

symbol_table.o: src/symbol_table.cpp src/symbol_table.h
	g++ -c src/symbol_table.cpp

record.o: src/record.cpp src/record.h
	g++ -c src/record.cpp

code_gen.o: src/code_gen.cpp src/code_gen.h
	g++ -c src/code_gen.cpp

clean:
	-rm *.o golf

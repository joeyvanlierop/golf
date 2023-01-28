#include <string>
#include "filereader.h"

void log(std::ostream& ostream, FileReader* fileReader, int line, int column, int width, std::string message);

void warning(FileReader* fileReader, int line, int column, int width, std::string message);

void error(FileReader* fileReader, int line, int column, int width, std::string message);

void print_line(std::ostream& ostream, std::string content);

void print_line(std::ostream& ostream, std::string content, int line);

std::string get_line(std::ifstream& filestream, int line);
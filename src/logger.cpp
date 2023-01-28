#include <iostream>
#include <iomanip>
#include "logger.h"

void log(std::ostream& ostream, FileReader* filereader, int line, int column, int width, std::string message) {
    // File name and location
    ostream << "--> " << filereader->filename << ":" << line << ":" << column << std::endl;

    // Padding
    print_line(ostream, "");

    // Error line
    auto error_line = get_line(filereader->filestream, line);
    print_line(ostream, error_line, line);

    // Explanation line
    message.insert(message.begin(), column, ' ');
    message.insert(message.begin() + column - 1, width, '^');
    print_line(ostream, message);
}

void warning(FileReader* filereader, int line, int column, int width, std::string message) {
    log(std::cerr, filereader, line, column, width, message);
}

void error(FileReader* filereader, int line, int column, int width, std::string message) {
    log(std::cerr, filereader, line, column, width, message);
    exit(EXIT_FAILURE);
}

void print_line(std::ostream& ostream, std::string content) {
    ostream << std::setw(2) << " " << "| " << content << std::endl;
}

void print_line(std::ostream& ostream, std::string content, int line) {
    ostream << std::setw(2) << std::left << line << "| " << content << std::endl;
}


std::string get_line(std::ifstream& filestream, int line) {
    std::string out;
    filestream.seekg(std::ios::beg);
    for(int i=0; i < line; i++){
        std::getline(filestream, out);
    }
    return out;
}



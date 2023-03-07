#pragma once

#include <string>
#include <tuple>
#include "filereader.h"

class Logger {
public:
    static void warning(FileReader* fileReader, int line, int column, int width, std::string message);
    static void error(FileReader* fileReader, int line, int column, int width, std::string message);

private:
    static int warnings;
    static const int max_warnings = 10;
    static void log(std::ostream& ostream, FileReader* fileReader, int line, int column, int width, std::string message);
    static void print_line(std::ostream& ostream, std::string content, int indent);
    static void print_line(std::ostream& ostream, std::string content, int line, int indent);
    static std::string get_line(std::ifstream& filestream, int line);
    static bool is_printable(const std::string &str);
    static std::tuple<std::string, int> normalize_line(const std::string &str, int end_column);
};


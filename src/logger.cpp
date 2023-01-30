#include <iostream>
#include <cmath>
#include <iomanip>
#include "logger.h"

/**
 * Log a message to an output stream along with the file name, line, and column of the error
 * @param ostream output stream to log the message to
 * @param filereader FileReader object containing the file name and stream
 * @param line line number of the error
 * @param column column number of the error
 * @param width width of the error mark
 * @param message the message to log
 */
void log(std::ostream& ostream, FileReader* filereader, int line, int column, int width, std::string message) {
    // File name and location
    ostream << "--> " << filereader->filename << ":" << line << ":" << column << std::endl;

    // Calculate how much to indent the vertical lines
    auto indent = log10(line) + 2;

    // Padding
    print_line(ostream, "", indent);

    // Error line
    auto error_line = get_line(filereader->filestream, line);
    print_line(ostream, error_line, line, indent);

    // Explanation line
    message.insert(message.begin(), column, ' ');
    message.insert(message.begin() + column - 1, 1, '^');
    message.insert(message.begin() + column, width - 1, '~');
    print_line(ostream, message, indent);
}

/**
 * Log a warning message to the standard error stream
 * @param filereader FileReader object containing the file name and stream
 * @param line line number of the error
 * @param column column number of the error
 * @param width width of the error mark
 * @param message the warning message to log
 */
void warning(FileReader* filereader, int line, int column, int width, std::string message) {
    log(std::cerr, filereader, line, column, width, "warning: " + message);
}

/**
 * Log an error message to the standard error stream and exits the program
 * @param filereader FileReader object containing the file name and stream
 * @param line line number of the error
 * @param column column number of the error
 * @param width width of the error mark
 * @param message the error message to log
 */
void error(FileReader* filereader, int line, int column, int width, std::string message) {
    log(std::cerr, filereader, line, column, width, "error: " + message);
    exit(EXIT_FAILURE);
}

/**
 * Print a string to an output stream with an aligned vertical line
 * @param ostream output stream to print the line to.
 * @param content the string to print.
 */
void print_line(std::ostream& ostream, std::string content, int indent) {
    ostream << std::setw(indent) << " " << "| " << content << std::endl;
}

/**
 * Print a string to an output stream with an aligned vertical line and line number
 * @param ostream output stream to print the line to
 * @param content the string to print
 * @param line line number to print
 */
void print_line(std::ostream& ostream, std::string content, int line, int indent) {
    ostream << std::setw(indent) << std::left << line << "| " << content << std::endl;
}

/**
 * Takes in an input file stream and a line number as input, and returns the content of that line as a string
 * @param filestream the input file stream to read from
 * @param line the line number to get the content from
 * @return a string representing the specified line of the input file
*/
std::string get_line(std::ifstream& filestream, int line) {
    std::string out;
    filestream.seekg(std::ios::beg);
    for(int i=0; i < line; i++){
        std::getline(filestream, out);
    }
    return out;
}



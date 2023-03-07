#include <iostream>
#include <cmath>
#include <iomanip>
#include "logger.h"

// Initialize warning counter
int Logger::warnings = 0;

/**
 * Log a message to an output stream along with the file name, line, and column of the error
 * @param ostream output stream to log the message to
 * @param filereader FileReader object containing the file name and stream
 * @param line line number of the error
 * @param column column number of the error
 * @param width width of the error mark
 * @param message the message to log
 */
void Logger::log(std::ostream &ostream, FileReader *filereader, int line, int column, int width, std::string message) {
    // File name and location
    ostream << "--> " << filereader->filename << ":" << line << ":" << column << std::endl;

    // Extract error line
    auto error_line = get_line(filereader->filestream, line);

    if (!is_printable(error_line)) {
        print_line(ostream, message, 0);
        return;
    }

    // Calculate how much to indent the vertical lines
    // log10 is used to determine the number of digits in the line number
    auto indent = log10(line) + 2;

    // Padding
    print_line(ostream, "", indent);

    // Error line
    if (!is_printable(error_line))
        error_line = "<encoding error>";
    auto [normalized_error_line, extra_indent] = normalize_line(error_line, column);
    print_line(ostream, normalized_error_line, line, indent);

    // Explanation line
    message.insert(message.begin(), column + extra_indent, ' ');
    message.insert(message.begin() + column + extra_indent - 1, 1, '^');
    message.insert(message.begin() + column + extra_indent, std::max(width - 1, 0), '~');
    print_line(ostream, message, indent);
}

/**
 * Log a warning message to the standard error stream
 * Error if too many warnings have been emitted
 * @param filereader FileReader object containing the file name and stream
 * @param line line number of the error
 * @param column column number of the error
 * @param width width of the error mark
 * @param message the warning message to log
 */
void Logger::warning(FileReader *filereader, int line, int column, int width, std::string message) {
    log(std::cerr, filereader, line, column, width, "warning: " + message);

    // Error out if we have too many warnings
    if (++warnings > max_warnings)
        error(filereader, line, column, width, "too many warnings");
}

/**
 * Log an error message to the standard error stream and exits the program
 * @param filereader FileReader object containing the file name and stream
 * @param line line number of the error
 * @param column column number of the error
 * @param width width of the error mark
 * @param message the error message to log
 */
void Logger::error(FileReader *filereader, int line, int column, int width, std::string message) {
    log(std::cerr, filereader, line, column, width, "error: " + message);
    exit(EXIT_FAILURE);
}

/**
 * Print a string to an output stream with an aligned vertical line
 * @param ostream output stream to print the line to.
 * @param content the string to print.
 */
void Logger::print_line(std::ostream &ostream, std::string content, int indent) {
    ostream << std::setw(indent) << " " << "| " << content << std::endl;
}

/**
 * Print a string to an output stream with an aligned vertical line and line number
 * @param ostream output stream to print the line to
 * @param content the string to print
 * @param line line number to print
 */
void Logger::print_line(std::ostream &ostream, std::string content, int line, int indent) {
    ostream << std::setw(indent) << std::left << line << "| " << content << std::endl;
}

/**
 * Takes in an input file stream and a line number as input, and returns the content of that line as a string
 * @param filestream the input file stream to read from
 * @param line the line number to get the content from
 * @return a string representing the specified line of the input file
*/
std::string Logger::get_line(std::ifstream &filestream, int line) {
    std::string out;
    filestream.seekg(std::ios::beg);
    for (int i = 0; i < line; i++) {
        std::getline(filestream, out);
    }
    return out;
}


/**
 * Determines if a given string is printable
 * Manually allow tabs ('\t') since isprint() does not include it
 * @param str the string to check
 * @return true if the string is printable, false otherwise
*/
bool Logger::is_printable(const std::string &str) {
    for (char c : str)
        if (!std::isprint(c) && c != '\t')
            return false;
    return true;
}

/**
 * Normalizes the given line to to accomodate tab escapes ('\t')
 * Assumes that a tab is 4 spaces (TODO: Maybe check is this is universal)
 * @param str the string to check
 * @param end_column the column where we stop checking
 * @return a tuple with the normalized string and additional indentation
*/
std::tuple<std::string, int> Logger::normalize_line(const std::string &str, int end_column) {
    std::string normalized = "";
    auto indent = 0;
    for (int i = 0; i < end_column; i++) {
        auto c = str[i];
        if (c == '\t') {
            normalized += "    ";
            indent += 4;
        } else {
            normalized += c;
        }
    }
    return {normalized, indent};
}



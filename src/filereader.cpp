#include <iostream>
#include <filesystem>
#include "filereader.h"

/**
 * Constructs a FileReader object with the given file name
 * @param filename the name of the file to be read
 */
FileReader::FileReader(const std::string &filename) : filename(filename) {
    filestream.open(filename);
    if (!filestream.is_open()){
        std::cerr << "File " + filename + " not found: " << std::filesystem::current_path() << std::endl;
        exit(EXIT_FAILURE);
    }
}

/**
 * Reads the entire contents of the file
 * @return the entire contents of the file as a string
 */
std::string FileReader::read_all() {
    std::string data((std::istreambuf_iterator<char>(filestream)),std::istreambuf_iterator<char>());
    return data;
}

/**
 * Destructor for the FileReader object
 * Closes the filestream associated with the object
 */
FileReader::~FileReader() {
    filestream.close();
}
#include <iostream>
#include <filesystem>
#include "filereader.h"

FileReader::FileReader(const std::string &filename) : filename(filename) {
    filestream.open(filename);
    if (!filestream.is_open()){
        std::cerr << "File " + filename + " not found: " << std::filesystem::current_path() << std::endl;
        exit(EXIT_FAILURE);
    }
}

std::string FileReader::read_all() {
    std::string data((std::istreambuf_iterator<char>(filestream)),std::istreambuf_iterator<char>());
    return data;
}

FileReader::~FileReader() {
    filestream.close();
}
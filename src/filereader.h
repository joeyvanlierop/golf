#pragma once

#include <string>
#include <fstream>

struct FileReader {
    FileReader(const std::string &filename);
    ~FileReader();
    std::string read_all();
    std::ifstream filestream;
    std::string filename;
};

#include <iostream>
#include <filesystem>
#include <fstream>
#include "file_input.h"

/**
 * Constructs a FileReader object with the given file name
 * @param filename the name of the file to be read
 */
FileInput::FileInput(const std::string &filename) : Input(filename) {

}

/**
 * Reads the entire contents of the file
 * @return the entire contents of the file as a string
 */
void FileInput::read() {
    std::ifstream filestream(Input::name);

    // Verify the file exists
    // TODO: Maybe throw error instead
    if (!filestream.is_open()){
        std::cerr << "File " + Input::name + " not found: " << std::filesystem::current_path() << std::endl;
        exit(EXIT_FAILURE);
    }

    Input::data = std::string((std::istreambuf_iterator<char>(filestream)),std::istreambuf_iterator<char>());
    filestream.close();
}

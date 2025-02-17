#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

class FileParser
{

public:
    FileParser() {}



private:
};

std::string getFileExtension(const std::string &filename)
{
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos)
    {
        return filename.substr(dotPos + 1);
    }
    return ""; // No extension found
}

std::string readFile(std::string fileName)
{
    std::ifstream inputFile(fileName);

    // Check if the file is successfully opened
    if (!inputFile.is_open())
    {
        throw std::runtime_error("Could not open file");
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    return buffer.str();
}

std::string writeCompressFile(){
    
}

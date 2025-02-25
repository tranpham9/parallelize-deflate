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

/**
 * Parses the fileName to get the FileExtention
 * @param filename the file to be parsed
 * @return A string of the file extention
 */
std::string getFileExtension(const std::string &filename)
{
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos)
    {
        return filename.substr(dotPos + 1);
    }
    return ""; // No extension found
}

/**
 * Reads the file to be compressed
 * @param fileName The name of the file that will be compressed
 * @return String of the entire file to be comressed
 */
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

    inputFile.close();
}

/**
 * Writes the compressed text to a file
 * @param compressedInfo The string of the compressed file
 * @param fileName This is a the original file name with the compression extention
 */
void writeCompressFile(std::string compressedInfo, std::string fileName)
{
    std::ofstream outfile(fileName);

    if(outfile.is_open()){
        throw std::runtime_error("Could not open file");
    }

    outfile << compressedInfo;

    outfile.close();
}

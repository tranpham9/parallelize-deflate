#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include <fstream>
#include <sstream>

using namespace std;

#define HUFFMAN_INFORMATION_SIZE 2

struct compressedFileData
{
    uint16_t treeSize;
    uint64_t bitCount;
    string tree;
    string compressedText;
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

    inputFile.close();

    return buffer.str();
}

/**
 * Reads the compressed file
 * Format of the compressed file
 * <treeSize> <bitCount> <treeData> <compressedText>
 * treeSize - uint16_t
 * bitCount - uint64_t
 * treeData - string with size of treeSize
 * compressedText - rest of file
 * @param fileName The name of the file to be read
 * @return compressedFileData The data of the compressed file
 */
compressedFileData readCompressedFile(string fileName)
{

    compressedFileData data;
    ifstream inputFile(fileName);

    // Check if the file is successfully opened
    if (!inputFile.is_open())
    {
        throw std::runtime_error("Could not open file");
    }

    inputFile.read(reinterpret_cast<char*>(&data.treeSize), sizeof(data.treeSize));
    inputFile.read(reinterpret_cast<char*>(&data.bitCount), sizeof(data.bitCount));

    char buf[data.treeSize];
    inputFile.read(buf, data.treeSize);
    data.tree = string(buf);

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    data.compressedText = buffer.str();

    return data;
}

/**
 * Writes the text to a file
 * @param text The string of the compressed file
 * @param fileName Name of the file to be writen to
 */
void writeFile(std::string text, std::string fileName)
{
    std::ofstream outfile(fileName);

    outfile << text;

    outfile.close();
}

/**
 * Writes the compressed file to a file
 * Format of the compressed file
 * <treeSize> <bitCount> <treeData> <compressedText>
 * treeSize - uint16_t
 * bitCount - uint64_t
 * treeData - string with size of treeSize
 * compressedText - rest of file
 * @param fileName The name of the file to be written to
 * @param treeSize The size of the tree
 * @param bitCount The number of bits in the compressed text
 * @param tree The tree as a string
 * @param compressedText The compressed text
 */
void writeCompressedFile(string fileName, uint16_t treeSize, uint64_t bitCount, string tree, string compressedText)
{
    std::ofstream outfile(fileName);

    if (!outfile.is_open())
    {
        cout << "Unable to write to the file: " << fileName << endl;
        return;
    }

    outfile.write(reinterpret_cast<const char*>(&treeSize), sizeof(treeSize));
    outfile.write(reinterpret_cast<const char*>(&bitCount), sizeof(bitCount));
    outfile << tree;
    outfile << compressedText;

    outfile.close();
}

// int main()
// {
//     string fileName = "writeTest.txt";
//     uint16_t treeSize = 15;
//     uint64_t bitCount = 60;
//     string tree = "123456789012345";
//     string compressedText = "123456789012345678901234567890123456789012345678901234567890";

//     writeCompressedFile(fileName, treeSize, bitCount, tree, compressedText);

//     compressedFileData fd = readCompressedFile(fileName);

//     cout << fd.treeSize << endl
//          << fd.bitCount << endl
//          << fd.tree << endl
//          << fd.compressedText << endl;

//     return 0;
// }
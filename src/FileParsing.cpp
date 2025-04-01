#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

#define HUFFMAN_INFORMATION_SIZE 2

struct compressedFileData {
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
string getFileExtension(const string &filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != string::npos) {
        return filename.substr(dotPos + 1);
    }
    return ""; // No extension found
}

/**
 * Reads the file to be compressed
 * @param fileName The name of the file that will be compressed
 * @return String of the entire file to be comressed
 */
string readFile(string fileName) {
    ifstream inputFile(fileName);

    // Check if the file is successfully opened
    if (!inputFile.is_open()) {
        throw runtime_error("Could not open file");
    }

    stringstream buffer;
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
compressedFileData readCompressedFile(string fileName) {
    ifstream inputFile(fileName);

    // Check if the file is successfully opened
    if (!inputFile.is_open()) {
        throw runtime_error("Could not open file");
    }

    compressedFileData fd;

    // Read the tree size and bit count
    inputFile.read(reinterpret_cast<char *>(&fd.treeSize), sizeof(fd.treeSize));
    inputFile.read(reinterpret_cast<char *>(&fd.bitCount), sizeof(fd.bitCount));

    // Read the tree data
    fd.tree.resize(fd.treeSize);
    inputFile.read(&fd.tree[0], fd.treeSize);

    // Read the compressed text
    string compressedText((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    fd.compressedText = compressedText;

    inputFile.close();

    return fd;
}

/**
 * Writes the text to a file
 * @param text The string of the compressed file
 * @param fileName Name of the file to be writen to
 */
void writeFile(string text, string fileName) {
    ofstream outfile(fileName);

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
void writeCompressedFile(string fileName, uint16_t treeSize, uint64_t bitCount, string tree, string compressedText) {
    ofstream outfile(fileName);

    if (!outfile.is_open()) {
        cout << "Unable to write to the file: " << fileName << endl;
        return;
    }

    outfile.write(reinterpret_cast<const char *>(&treeSize), sizeof(treeSize));
    outfile.write(reinterpret_cast<const char *>(&bitCount), sizeof(bitCount));
    outfile << tree;
    outfile << compressedText;

    outfile.close();
}

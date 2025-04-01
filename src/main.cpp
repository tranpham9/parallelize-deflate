#include "FileParsing.cpp"
#include "Huffman.cpp"
#include "LZ77.cpp"
#include <iostream>
#include <thread>
#include <vector>

#define COMPRESSED_FILE_EXTENTION "pdc"
using namespace std;

void handleFile(string fileName) {

    cout << "Handling file: " << fileName << endl;

    LZ77 lz77;
    if (!getFileExtension(fileName).compare(COMPRESSED_FILE_EXTENTION)) {
        // decompress
        cout << fileName << ":decompression" << endl;
        compressedFileData fileData = readCompressedFile(fileName);
        int util = 0;

        string compressedBinaryString = asciiToBinary(fileData.compressedText, fileData.bitCount);
        HuffmanNode *huffmanTree = stringAsTree(nullptr, fileData.tree, util); // convert string tree into huffman tree

        if (!huffmanTree) {
            cerr << "Decompression failed: Huffman tree is null." << endl;
            return;
        }

        string huffman_decompress = HuffmanDecompress(compressedBinaryString, huffmanTree);

        string final_decompressed_text = lz77.decompress(huffman_decompress);

        string name = "copy_" + fileName.substr(0, fileName.size() - 4);

        writeFile(final_decompressed_text, name);
    } else {
        // compress
        cout << fileName << ": compression" << endl;
        string fileData = readFile(fileName);

        auto t1 = chrono::system_clock::now();

        // lz77 compress
        string lz_compressed = lz77.compress(fileData);
        auto t2 = chrono::system_clock::now();

        // Huffman Compress
        pair<string, HuffmanNode *> p = HuffmanCodes(lz_compressed);
        auto t3 = chrono::system_clock::now();

        // creates the different parts of the compressed file
        string tree;
        treeAsString(p.second, tree);
        uint16_t treeLength = tree.length();

        size_t compressLength = p.first.length();
        string compressedASCII = binaryToASCII(p.first);

        // forms the compressed file Name
        string compressedFileName = fileName + "." + COMPRESSED_FILE_EXTENTION;

        writeCompressedFile(compressedFileName, treeLength, compressLength, tree, compressedASCII);

        auto t4 = chrono::system_clock::now();

        auto LZ77Time = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
        auto HuffmanTime = chrono::duration_cast<chrono::milliseconds>(t3 - t2).count();
        auto TotalTime = chrono::duration_cast<chrono::milliseconds>(t4 - t1).count();
    }

    cout << fileName << ": complete" << endl;
}

// Program arguments:
//      1. FileName

int main(int argc, char *argv[]) {

    if (argc < 2) {
        cout << "no input paramater was inputed" << endl;
        return EXIT_FAILURE;
    }

    vector<thread> threads;
    for (size_t i = 1; i < argc; i++) {
        string fileName = argv[i];
        threads.emplace_back(handleFile, fileName);
    }

    for (auto &&i : threads) {
        i.join();
    }

    return EXIT_SUCCESS;
}

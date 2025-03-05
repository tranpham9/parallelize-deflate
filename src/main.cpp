#include "FileParsing.cpp"
#include "Huffman.cpp"
#include "LZ77.cpp"
#include <iostream>
#include <thread>
#include <vector>

#define COMPRESSED_FILE_EXTENTION "pdc"
using namespace std;

void handleFile(std::string fileName) {
    std::string fileData = readFile(fileName);

    LZ77 lz77;
    if (!getFileExtension(fileName).compare(COMPRESSED_FILE_EXTENTION)) {
        // decompress
        cout << fileName << ":decompression" << endl;

        /*
            Huffman decompression
        */
        std::string huffman_decompress = fileData;

        std::string final_decompressed_text = lz77.decompress(fileData);

        std::string name = fileName.substr(0, fileName.size() - 4);

        writeFile(final_decompressed_text, name);
    } else {
        // compress
        cout << fileName << ": compression" << endl;

        std::string lz_compressed = lz77.compress(fileData);

        /*
            Huffman Compress
        */
        pair<string, HuffmanNode *> p = HuffmanCodes(lz_compressed);
        HuffmanNode *root = p.second;

        writeFile(p.first, fileName + "." + COMPRESSED_FILE_EXTENTION);
    }

    cout << fileName << ": complete" << endl;
}

// Program arguments:
//      1. FileName

int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cout << "no input paramater was inputed" << std::endl;
        return EXIT_FAILURE;
    }

    vector<thread> threads;

    for (size_t i = 1; i < argc; i++) {
        std::string fileName = argv[i];
        threads.emplace_back(handleFile, fileName);
        // handleFile(fileName);
    }

    for (auto &&i : threads) {
        i.join();
    }

    return EXIT_SUCCESS;
}

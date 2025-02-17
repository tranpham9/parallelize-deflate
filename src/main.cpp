#include <iostream>
#include "LZ77.cpp"
#include "FileParsing.cpp"



#define COMPRESSED_FILE_EXTENTION "pdc"


// Program arguments: 
//      1. FileName

int main(int argc, char *argv[]){


    if (argc < 2){
        std::cout << "no input paramater was inputed" << std::endl;
        return EXIT_FAILURE;
    }
    std::string fileName = argv[1];
    std::string fileData = readFile(fileName);

    LZ77 lz77;
    if(getFileExtension(fileName).compare(COMPRESSED_FILE_EXTENTION)){
        // decompress

        /*
            Huffman decompression
        */
        std::string huffman_decompress = "";

        std::string final_decompressed_text = lz77.decompress(huffman_decompress);

        /*
            write to decompressed file
        */
    }else{
        //compress

        std::string lz_compressed = lz77.compress(fileData);

        /*
            Huffman Compress
        */

        /*
            write to compressed file
        */
    }

    // read file
    std::string test = "aabcbbabc";

    // LZ77 compression algorithm
    // LZ77 compressor;
    // std::string LZ_compressed = compressor.compress(test);

    std::cout << test << std::endl;
    // std::cout << LZ_compressed << std::endl;


    // huffman compression on LZ77 output

    return EXIT_SUCCESS;
}







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

    return EXIT_SUCCESS;
}







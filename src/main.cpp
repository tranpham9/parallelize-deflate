#include <iostream>
#include "LZ77.cpp"
#include "Huffman.cpp"
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
    if(!getFileExtension(fileName).compare(COMPRESSED_FILE_EXTENTION)){
        // decompress
        cout << "decompression" << endl;

        /*
            Huffman decompression
        */
        std::string huffman_decompress = fileData;

        std::string final_decompressed_text = lz77.decompress(fileData);

        std::string name = fileName.substr(0, fileName.size()-4);
        
        writeFile(final_decompressed_text, name);
    }else{
        //compress
        cout << "compression" << endl;

        std::string lz_compressed = lz77.compress(fileData);

        /*
            Huffman Compress
        */
       std::string huffman_compression_text = lz_compressed;

        writeFile(huffman_compression_text, fileName + "." + COMPRESSED_FILE_EXTENTION);
    }

    cout << "complete" << endl;

    return EXIT_SUCCESS;
}







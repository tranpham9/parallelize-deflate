# DEFLATE

Parallel Lossless File Compression

## Usage of the program

The program is written in C++, with version 20. Compiling the program will be done using g++ (command for compiling will be left below). Once compiled, running the program requires at least one parameter. The parameters for the program are paths to a file, more than one file can be placed as parameters (No Folders). This program does not output anything, however, new files will be created. The file created from the program will depend on the file inputted. The files will be created in the current directory. If a normal file is imputed the compressed file will have the same name but with the .pdc extension. If a compressed file is inputted the file that is created will be a file with the same name as the original.

Command to compile: <br>
`make`

Command to run the program and compress/decompress: <br>
`./build/deflate.out [directory to file to be compressed or decompressed]`

## Necessary Files

**FileParsing.cpp** - Parses the file to be processed <br>
**Huffman.cpp** - Implements Huffman encoding and decoding <br>
**KMPPatternMatching.cpp** - Implments a modified version of KMP to be used in LZ77 <br>
**LZ77.cpp** - Implements LZ77 compression and decompression <br>

#include <fstream>
#include <string>
#include <vector>
#include <stdint.h>
#include "KMPPatternMatch.cpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>

struct Token
{
    uint8_t length;
    uint8_t distance;
    char character;

    Token(uint8_t _length, uint8_t _distance, char _character)
    {
        length = _length;
        distance = _distance;
        character = _character;
    }

    Token() {}
};

class LZ77
{

private:
static const size_t BUF_LENGTH = 255;
#define BUFFER_LENGTH 255
#define COMPARE_WINDOW 255

    std::vector<Token> tokenText;

    void createTokenArray(std::string &text)
    {
        KMP_Search kmp;

        // loops throught the entire text(aka. file)
        for (size_t i = 0; i < text.size();)
        {
            size_t windowStart = std::max((long int)0, (long int)(i - BUFFER_LENGTH));
            // int windowEnd = (i-1) < 0 ? 0 : (i-1);
            size_t windowEnd = i;
            size_t patternEnd =  std::min((text.length()-1) , (i+COMPARE_WINDOW));


            // std::string pattern = text.substr(i, patternEnd);
            // std::cout << "pattern\t(" << pattern.length() << "): " << pattern << endl;
            // std::string compareWindow = text.substr(windowStart, windowEnd);
            // std::cout << "window \t(" << compareWindow.length() << "): " << compareWindow << endl;

            struct PatternResult patternResult = kmp.search(text, i, patternEnd, windowStart, i);

            // std::cout << "PatternResult[" << (int)patternResult.index << ", " << (int)patternResult.matchLength << "]" << endl;

            tokenText.emplace_back(Token(patternResult.index, patternResult.matchLength, text[i + patternResult.matchLength]));

            i = i + patternResult.matchLength + 1;
        }
    }

    std::string tokenArrayToString()
    {
        std::vector<char> compressedText;
        for (auto &&token : tokenText)
        {
            compressedText.emplace_back(token.length);
            compressedText.emplace_back(token.distance);
            compressedText.emplace_back(token.character);
            // std::cout << "<" << (int)token.length << ", " << (int)token.distance << ", " << token.character << ">" << std::endl;
        }
        return std::string(compressedText.begin(), compressedText.end());
    }

    void stringToTokenArray(std::string &compressedText)
    {
        for (size_t i = 0; i < compressedText.size(); i = i + 3)
        {
            Token temp;
            temp.length = compressedText[i];
            temp.distance = compressedText[i + 1];
            temp.character = compressedText[i + 2];

            tokenText.emplace_back(temp);

            // std::cout << "<" << (int)temp.length << ", " << (int)temp.distance << ", " << temp.character << ">\t" << endl;
        }
    }

public:
    LZ77()
    {
    }

    std::string compress(std::string &text)
    {
        if (text.empty())
        {
            // check is compression is avaliable
            return NULL;
        }
        tokenText.clear();

        createTokenArray(text);
        return tokenArrayToString();
    }

    std::string decompress(std::string &compressedText)
    {
        if (compressedText.empty())
        {
            return NULL;
        }
        tokenText.clear();
        std::vector<char> decompressedText;

        stringToTokenArray(compressedText);

        for (size_t i = 0; i < tokenText.size(); i++)
        {
            uint8_t length = tokenText[i].length;
            uint8_t distance = tokenText[i].distance;

            // std::cout << "length: " << (int)length << ", distance: " << (int)distance << endl; 

            long startingPosition = decompressedText.size() - length;

            for (size_t j = startingPosition; j < startingPosition + distance; j++)
            {
                // std::cout << (int)i << ", " << (int)j << endl;
                if (distance == 0){
                    break;
                }
                decompressedText.emplace_back(decompressedText[j]);
            }

            decompressedText.emplace_back(tokenText[i].character);
        }

        return std::string(decompressedText.begin(), decompressedText.end());
    }
};

int main()
{

    auto start = std::chrono::system_clock::now();

   std::ifstream f("/home/zfuser/myucf/COP4520/parallelize-deflate/input.txt");

    // Check if the file is successfully opened
   if (!f.is_open())
   {
       std::cout << "Error opening the file!" << std::endl;
       return EXIT_FAILURE;
   }

    std::string test = "aabcbbabc";
    // std::vector<std::byte> bytes;
    std::getline(f, test);
    f.close();

    // std::cout << "Text: " << test << endl;
    // std::cout << "Original length:\t" << test.length() << std::endl;

    // LZ77 compression algorithm
    LZ77 compressor;

    auto t1 = std::chrono::system_clock::now();
    std::string LZ_compressed = compressor.compress(test);
    auto t2 = std::chrono::system_clock::now();

    // std::cout << "Compressed length:\t" << LZ_compressed.length() << std::endl;

    auto t3 = std::chrono::system_clock::now();
    std::string LZ_decompressed = compressor.decompress(LZ_compressed);
    auto t4 = std::chrono::system_clock::now();

    // std::cout << "Decompressed length:\t" << LZ_decompressed.length() << std::endl;


    // std::cout << "Compessed Text\t\t"<< LZ_compressed << endl;
    std::cout << LZ_decompressed;

    // cout << "\n\n";
    // if (!LZ_compressed.compare(LZ_decompressed)){
    //     cout << "True" << endl;
    // }else{
    //     cout << "False" << endl;
    // }

    auto fileReatTime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - start).count();
    auto cmpTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    auto decmpTime = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count();
    auto totaltime = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - start).count();

    // std::cout << "===================================================" << std::endl;
    // std::cout << "File Read Time:\t\t" << fileReatTime << std::endl;
    // std::cout << "Compression Time:\t" << cmpTime << std::endl;
    // std::cout << "Decompression Time:\t" << decmpTime << std::endl;
    // std::cout << "Total Time:\t\t" << totaltime << std::endl;
}

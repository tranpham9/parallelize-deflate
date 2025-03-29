#include <fstream>
#include <string>
#include <vector>
#include <stdint.h>
#include "KMPPatternMatch.cpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <thread>

struct Token
{
    uint16_t length;
    uint8_t distance;
    char character;

    Token(uint16_t _length, uint8_t _distance, char _character)
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
#define THREAD_COUNT 8            // must be greater than or equal to 1
#define DECOMPRESS_TOKEN_VECTOR 0 // must be less that thread count

#define TOKEN_BYTE_SIZE 4   // DO NOT CHANGE
#define BUFFER_LENGTH 65535 // max value 65535
#define COMPARE_WINDOW 255  // max value 255

    std::vector<Token> tokenText[THREAD_COUNT];

    void createTokenArray(std::string &text, size_t start, size_t end, int tokenVector)
    {
        KMP_Search kmp;

        // loops throught the entire text(aka. file)
        for (size_t i = start; i < end;)
        {
            size_t windowStart = std::max((long int)start, (long int)(i - BUFFER_LENGTH));
            // int windowEnd = (i-1) < 0 ? 0 : (i-1);
            // size_t windowEnd = i;
            size_t patternEnd = std::min((end - 1), (i + COMPARE_WINDOW));

            // std::string pattern = text.substr(i, patternEnd);
            // std::cout << "pattern\t(" << pattern.length() << "): " << pattern << endl;
            // std::string compareWindow = text.substr(windowStart, windowEnd);
            // std::cout << "window \t(" << compareWindow.length() << "): " << compareWindow << endl;

            struct PatternResult patternResult = kmp.search(text, i, patternEnd, windowStart, i);

            // std::cout << "PatternResult[" << (int)patternResult.index << ", " << (int)patternResult.matchLength << "]" << endl;

            tokenText[tokenVector].emplace_back(Token(patternResult.index, patternResult.matchLength, text[i + patternResult.matchLength]));

            i = i + patternResult.matchLength + 1;
        }
    }

    std::string tokenArrayToString()
    {
        std::vector<char> compressedText;
        for (auto &&tokens : tokenText)
        {
            for (auto &&token : tokens)
            {
                compressedText.emplace_back((token.length >> 8) & 0xff); // high bytes
                compressedText.emplace_back(token.length & 0xff);        // low bytes

                compressedText.emplace_back(token.distance);
                compressedText.emplace_back(token.character);
                // std::cout << "<" << (int)token.length << ", " << (int)token.distance << ", " << token.character << ">" << std::endl;
            }
        }
        return std::string(compressedText.begin(), compressedText.end());
    }

    void stringToTokenArray(std::string &compressedText)
    {
        for (size_t i = 0; i < compressedText.size(); i = i + TOKEN_BYTE_SIZE)
        {
            Token temp;
            temp.length = ((uint16_t)compressedText[i]) << 8; // high bytes
            temp.length |= compressedText[i + 1] & 0xff;      // low bytes

            temp.distance = compressedText[i + 2];
            temp.character = compressedText[i + 3];

            tokenText[DECOMPRESS_TOKEN_VECTOR].emplace_back(temp);

            // std::cout << "<" << (uint16_t)temp.length << ", " << (int)temp.distance << ", " << temp.character << ">\t" << endl;
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

        vector<thread> threads;
        size_t workLoad = text.length() / THREAD_COUNT;
        for (size_t i = 0; i < THREAD_COUNT-1; i++)
        {
            threads.emplace_back(&LZ77::createTokenArray, this, std::ref(text), i * workLoad, (i + 1) * workLoad, i);
        }
        // last thread takes the rest of the text
        threads.emplace_back(&LZ77::createTokenArray, this, std::ref(text), (THREAD_COUNT - 1) * workLoad, text.length(), THREAD_COUNT - 1);
        
        

        for (auto &&task : threads)
        {
            task.join();
        }

        // createTokenArray(text, 0, text.length());
        return tokenArrayToString();
    }

    std::string decompress(std::string &compressedText)
    {
        if (compressedText.empty())
        {
            return NULL;
        }
        std::vector<char> decompressedText;

        stringToTokenArray(compressedText);

        std::vector<Token> tokens = tokenText[DECOMPRESS_TOKEN_VECTOR];
        for (size_t i = 0; i < tokens.size(); i++)
        {
            uint16_t length = tokens[i].length;
            uint8_t distance = tokens[i].distance;

            // std::cout << "length: " << (int)length << ", distance: " << (int)distance << endl;

            size_t startingPosition = decompressedText.size() - length;

            for (size_t j = startingPosition; j < startingPosition + distance; j++)
            {
                // std::cout << (int)i << ", " << (int)j << endl;
                if (distance == 0)
                {
                    break;
                }
                decompressedText.emplace_back(decompressedText[j]);
            }

            decompressedText.emplace_back(tokens[i].character);
        }

        return std::string(decompressedText.begin(), decompressedText.end());
    }
};

// int main()
// {

//     auto start = std::chrono::system_clock::now();

//    std::ifstream f("/home/zfuser/myucf/COP4520/parallelize-deflate/input.txt");

//     // Check if the file is successfully opened
//    if (!f.is_open())
//    {
//        std::cout << "Error opening the file!" << std::endl;
//        return EXIT_FAILURE;
//    }

//     std::string test = "aabcbbabc";

//     std::stringstream buffer;
//     buffer << f.rdbuf();
//     test = buffer.str();

//     f.close();

//     // LZ77 compression algorithm
//     LZ77 compressor;

//     auto t1 = std::chrono::system_clock::now();
//     std::string LZ_compressed = compressor.compress(test);
//     auto t2 = std::chrono::system_clock::now();

//     auto t3 = std::chrono::system_clock::now();
//     std::string LZ_decompressed = compressor.decompress(LZ_compressed);
//     auto t4 = std::chrono::system_clock::now();

//     // std::cout << test;
//     // std::cout << LZ_compressed;
//     // std::cout << LZ_decompressed;

//     std::cout << "===================================================" << std::endl;
//     std::cout << "Original length:\t" << test.length() << std::endl;
//     std::cout << "Compressed length:\t" << LZ_compressed.length() << std::endl;
//     std::cout << "Decompressed length:\t" << LZ_decompressed.length() << std::endl;

//     auto fileReatTime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - start).count();
//     auto cmpTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
//     auto decmpTime = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count();
//     auto totaltime = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - start).count();

//     std::cout << "===================================================" << std::endl;
//     std::cout << "File Read Time:\t\t" << fileReatTime << std::endl;
//     std::cout << "Compression Time:\t" << cmpTime << std::endl;
//     std::cout << "Decompression Time:\t" << decmpTime << std::endl;
//     std::cout << "Total Time:\t\t" << totaltime << std::endl;
// }

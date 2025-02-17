#include <fstream>
#include <string>
#include <vector>
#include "KMPPatternMatch.cpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>

struct Token
{
    u_int8_t length;
    u_int8_t distance;
    char character;

    Token(u_int8_t _length, u_int8_t _distance, char _character)
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
#define BUFFER_LENGTH 256
#define COMPARE_WINDOW 256

    std::vector<Token> tokenText;

    void createTokenArray(std::string &text)
    {

        for (size_t i = 0; i < text.size();)
        {
            int startWindow = 0 < (i - BUFFER_LENGTH) ? 0 : i - BUFFER_LENGTH; // max(0, i-BUFFER_LENGTH)
            u_int8_t largestMatch = 0;
            u_int8_t iderateAmount = 0;

            for (size_t j = startWindow; j < i && i - j > largestMatch; j++)
            {
                u_int8_t matchLength = 0;

                while (text[j + matchLength] == text[i + matchLength])
                {
                    matchLength++;
                }

                if (matchLength >= largestMatch)
                {
                    largestMatch = matchLength;
                    iderateAmount = i - j;
                }
            }
            std::cout << "<" << (int)iderateAmount << ", " << (int)largestMatch << ", " << text[i + largestMatch] << ">" << std::endl;
            tokenText.emplace_back(Token(iderateAmount, largestMatch, text[i + largestMatch]));

            i = i + largestMatch + 1;
        }
    }

    void createTokenArray2(std::string &text)
    {
        KMP_Search kmp;

        // loops throught the entire text(aka. file)
        for (size_t i = 0; i < text.size();)
        {
            int startWindow = 0 < (i - BUFFER_LENGTH) ? 0 : i - BUFFER_LENGTH; // max(0, i-BUFFER_LENGTH)

            std::string pattern = text.substr(i, COMPARE_WINDOW);
            // std::cout << "pattern\t(" << pattern.length() << "): " << pattern << endl;
            std::string compareWindow = text.substr(startWindow, i);
            // std::cout << "window \t(" << compareWindow.length() << "): " << compareWindow << endl;

            struct PatternResult patternResult = kmp.search(pattern, compareWindow);

            // std::cout << "PatternResult[" << (int)patternResult.index << ", " << (int)patternResult.matchLength << "]" << endl;

            // std::cout << "<" << (int)iderateAmount << ", " << (int)largestMatch << ", " << text[i + largestMatch+1] << ">" << std::endl;
            tokenText.emplace_back(Token(patternResult.index, patternResult.matchLength, text[i + patternResult.matchLength]));

            i = i + patternResult.matchLength + 1;
        }
    }

    std::string tokenArrayToString()
    {
        std::vector<char> compressedText;
        for (auto &&token : tokenText)
        {
            compressedText.emplace_back((char)token.length);
            compressedText.emplace_back((char)token.distance);
            compressedText.emplace_back(token.character);
            // std::cout << "<" << (int)token.length << ", " << (int)token.distance << ", " << token.character << ">" << std::endl;
        }
        return std::string(compressedText.begin(), compressedText.end());
    }

    void stringToTokenArray(std::string &compressedText)
    {
        for (size_t i = 0; i < compressedText.size(); i = i + 4)
        {
            Token temp;
            temp.length = compressedText[i];
            temp.distance = compressedText[i + 1];
            temp.character = compressedText[i + 2];

            tokenText.emplace_back(temp);

            // std::cout << "<" << (int)temp.length << ", " << (int)temp.distance << ", " << temp.character << ">\t";
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

        createTokenArray2(text);
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
            char length = tokenText[i].length;
            char distance = tokenText[i].distance;

            long startingPosition = decompressedText.size() - length;

            for (size_t j = startingPosition; j < startingPosition + distance; j++)
            {
                decompressedText.emplace_back(decompressedText[j]);
                // std::cout << "(" << i << ", " << j << ")\tcopy:\t" << text[j] << std::endl;
            }

            decompressedText.emplace_back(tokenText[i].character);
            // std::cout << "(" << i << ")\tnew:\t" << tokenText[i].character << std::endl;
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
    std::cout << "Original length:\t" << test.length() << std::endl;

    // LZ77 compression algorithm
    LZ77 compressor;

    auto t1 = std::chrono::system_clock::now();
    std::string LZ_compressed = compressor.compress(test);
    auto t2 = std::chrono::system_clock::now();

    std::cout << "Compressed length:\t" << LZ_compressed.length() << std::endl;

    auto t3 = std::chrono::system_clock::now();
    std::string LZ_decompressed = compressor.decompress(LZ_compressed);
    auto t4 = std::chrono::system_clock::now();

    // std::cout << LZ_compressed << endl;

    auto fileReatTime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - start).count();
    auto cmpTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    auto decmpTime = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count();
    auto totaltime = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - start).count();

    std::cout << "===================================================" << std::endl;
    std::cout << "File Read Time:\t\t" << fileReatTime << std::endl;
    std::cout << "Compression Time:\t" << cmpTime << std::endl;
    std::cout << "Decompression Time:\t" << decmpTime << std::endl;
    std::cout << "Total Time:\t\t" << totaltime << std::endl;
}

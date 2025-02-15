#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>

struct Token
{
    char length;
    char distance;
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

    std::vector<char> text;
    std::vector<Token> tokenText;
    std::vector<char> compressedText;

    void createTokenArray()
    {

        for (size_t i = 0; i < text.size();)
        {
            u_int8_t iderateAmount = 0;

            int startWindow = 0 < (i - BUFFER_LENGTH) ? 0 : i - BUFFER_LENGTH; // max(0, i-BUFFER_LENGTH)
            u_int8_t largestMatch = 0;

            for (size_t j = startWindow; j < i && i - j > largestMatch; j++)
            {
                int matchLength = 0;

                while (text[j + matchLength] == text[i + matchLength])
                {
                    matchLength++;
                }

                if (matchLength > largestMatch)
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

    void tokenArrayToString()
    {
        for (auto &&token : tokenText)
        {
            compressedText.emplace_back((char)token.length);
            compressedText.emplace_back((char)token.distance);
            compressedText.emplace_back(token.character);
        }
    }

    void stringToTokenArray()
    {
        for (size_t i = 0; i < compressedText.size(); i = i + 3)
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

    std::string compress(std::string _text)
    {
        if (_text.empty())
        {
            // check is compression is avaliable
            return NULL;
        }
        resetInfo();

        text = std::vector(_text.begin(), _text.end());

        for (size_t i = 0; i < text.size();)
        {
            u_int8_t iderateAmount = 0;

            int startWindow = 0 < (i - BUFFER_LENGTH) ? 0 : i - BUFFER_LENGTH; // max(0, i-BUFFER_LENGTH)
            u_int8_t largestMatch = 0;

            for (size_t j = startWindow; j < i && i - j > largestMatch; j++)
            {
                u_int8_t matchLength = 0;

                while (text[j + matchLength] == text[i + matchLength])
                {
                    matchLength++;
                }

                if (matchLength > largestMatch)
                {
                    largestMatch = matchLength;
                    iderateAmount = i - j;
                }
            }
            // std::cout << "<" << (int)iderateAmount << ", " << (int)largestMatch << ", " << _text[i + largestMatch] << ">" << std::endl;
            tokenText.emplace_back(Token(iderateAmount, largestMatch, _text[i + largestMatch]));

            i = i + largestMatch + 1;
        }

        tokenArrayToString();

        return std::string(compressedText.begin(), compressedText.end());
    }

    std::string decompress(std::string _compressedText)
    {
        if (_compressedText.empty())
        {
            return NULL;
        }
        resetInfo();

        compressedText = std::vector<char>(_compressedText.begin(), _compressedText.end());

        stringToTokenArray();

        for (size_t i = 0; i < tokenText.size(); i++)
        {
            char length = tokenText[i].length;
            char distance = tokenText[i].distance;

            long startingPosition = text.size() - length;

            for (size_t j = startingPosition; j < startingPosition + distance; j++)
            {
                text.emplace_back(text[j]);
                // std::cout << "(" << i << ", " << j << ")\tcopy:\t" << text[j] << std::endl;
            }

            text.emplace_back(tokenText[i].character);
            // std::cout << "(" << i << ")\tnew:\t" << tokenText[i].character << std::endl;
        }

        return std::string(text.begin(), text.end());
    }

    void resetInfo()
    {
        text.clear();
        tokenText.clear();
        compressedText.clear();
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

    std::string test = "";
    std::getline(f, test);
    f.close();







    std::cout << "Original length: " << test.length() << std::endl;

    // LZ77 compression algorithm
    LZ77 compressor;

    auto t1 = std::chrono::system_clock::now();
    std::string LZ_compressed = compressor.compress(test);
    auto t2 = std::chrono::system_clock::now();


    std::cout << "Compressed length: " << LZ_compressed.length() << std::endl;

    auto t3 = std::chrono::system_clock::now();
    std::string LZ_decompressed = compressor.decompress(LZ_compressed);
    auto t4 = std::chrono::system_clock::now();


    auto fileReatTime = std::chrono::duration_cast<std::chrono::milliseconds>(t1-start).count();
    auto cmpTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
    auto decmpTime = std::chrono::duration_cast<std::chrono::milliseconds>(t4-t3).count();
    auto totaltime = std::chrono::duration_cast<std::chrono::milliseconds>(t4-start).count();

    std::cout << "===================================================" << std::endl;
    std::cout << "File Read Time:\t" << fileReatTime << std::endl;
    std::cout << "Compression Time:\t" << cmpTime << std::endl;
    std::cout << "Decompression Time:\t" << decmpTime << std::endl;
    std::cout << "Total Time:\t" << totaltime << std::endl;
}

#include "KMPPatternMatch.cpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>
#include <thread>
#include <vector>

using namespace std;

struct Token {
    uint16_t length;
    uint8_t distance;
    char character;

    Token(uint16_t _length, uint8_t _distance, char _character) {
        length = _length;
        distance = _distance;
        character = _character;
    }

    Token() {}
};

class LZ77 {

private:
#define THREAD_COUNT 8            // must be greater than or equal to 1
#define DECOMPRESS_TOKEN_VECTOR 0 // must be less that thread count

#define TOKEN_BYTE_SIZE 4   // DO NOT CHANGE
#define BUFFER_LENGTH 65535 // max value 65535
#define COMPARE_WINDOW 255  // max value 255

    vector<Token> tokenText[THREAD_COUNT];

    void createTokenArray(string &text, size_t start, size_t end, int tokenVector) {
        KMP_Search kmp;

        // loops throught the entire text(aka. file)
        for (size_t i = start; i < end;) {
            size_t windowStart = max((long int)start, (long int)(i - BUFFER_LENGTH));
            size_t patternEnd = min((end - 1), (i + COMPARE_WINDOW));

            struct PatternResult patternResult = kmp.search(text, i, patternEnd, windowStart, i);

            tokenText[tokenVector].emplace_back(Token(patternResult.index, patternResult.matchLength, text[i + patternResult.matchLength]));

            i = i + patternResult.matchLength + 1;
        }
    }

    string tokenArrayToString() {
        vector<char> compressedText;
        for (auto &&tokens : tokenText) {
            for (auto &&token : tokens) {
                compressedText.emplace_back((token.length >> 8) & 0xff); // high bytes
                compressedText.emplace_back(token.length & 0xff);        // low bytes

                compressedText.emplace_back(token.distance);
                compressedText.emplace_back(token.character);
            }
        }
        return string(compressedText.begin(), compressedText.end());
    }

    void stringToTokenArray(string &compressedText) {
        for (size_t i = 0; i < compressedText.size(); i = i + TOKEN_BYTE_SIZE) {
            Token temp;
            temp.length = ((uint16_t)compressedText[i]) << 8; // high bytes
            temp.length |= compressedText[i + 1] & 0xff;      // low bytes

            temp.distance = compressedText[i + 2];
            temp.character = compressedText[i + 3];

            tokenText[DECOMPRESS_TOKEN_VECTOR].emplace_back(temp);
        }
    }

public:
    LZ77() {
    }

    string compress(string &text) {
        if (text.empty()) {
            // check is compression is avaliable
            return NULL;
        }

        vector<thread> threads;
        size_t workLoad = text.length() / THREAD_COUNT;
        for (size_t i = 0; i < THREAD_COUNT - 1; i++) {
            threads.emplace_back(&LZ77::createTokenArray, this, ref(text), i * workLoad, (i + 1) * workLoad, i);
        }
        // last thread takes the rest of the text
        threads.emplace_back(&LZ77::createTokenArray, this, ref(text), (THREAD_COUNT - 1) * workLoad, text.length(), THREAD_COUNT - 1);

        for (auto &&task : threads) {
            task.join();
        }

        return tokenArrayToString();
    }

    string decompress(string &compressedText) {
        if (compressedText.empty()) {
            return NULL;
        }
        vector<char> decompressedText;

        stringToTokenArray(compressedText);

        vector<Token> tokens = tokenText[DECOMPRESS_TOKEN_VECTOR];
        for (size_t i = 0; i < tokens.size(); i++) {
            uint16_t length = tokens[i].length;
            uint8_t distance = tokens[i].distance;

            size_t startingPosition = decompressedText.size() - length;

            for (size_t j = startingPosition; j < startingPosition + distance; j++) {
                if (distance == 0) {
                    break;
                }
                decompressedText.emplace_back(decompressedText[j]);
            }

            decompressedText.emplace_back(tokens[i].character);
        }

        return string(decompressedText.begin(), decompressedText.end());
    }
};
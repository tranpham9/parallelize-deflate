#include <iomanip>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>

using namespace std;

class HuffmanNode {
public:
    char data;
    unsigned freq;
    HuffmanNode *left;
    HuffmanNode *right;

    // Constructor for internal node
    HuffmanNode(char data, unsigned freq, HuffmanNode *left, HuffmanNode *right) {
        this->data = data;
        this->freq = freq;
        this->left = left;
        this->right = right;
    }

    // Constructor for leaf node
    HuffmanNode(char data, unsigned freq) {
        this->data = data;
        this->freq = freq;
        left = right = nullptr;
    }
};

// Function object to be used as a custom comparator
struct compare {
    bool operator()(HuffmanNode *left, HuffmanNode *right) {
        return (left->freq > right->freq);
    }
};

// method to count : array of size 256 to tally characters
// rewrite the build tree method to take in an array and turn the characters and frequency to string
// build tree method should be reusable for encoding and decoding - build a heap

// freq should be of size 256
void countFrequencies(const string &data, vector<unsigned int> &freq) {
    fill(freq.begin(), freq.end(), 0);

    for (unsigned char c : data) {
        freq[c]++;
    }
}

// Build the Huffman Tree by repeatedly combining two nodes with the lowest frequncies
// into a new until there is only one node left. The last node will become the root of the tree
HuffmanNode *buildTree(const vector<unsigned> &freq) {
    // Minheap to store the nodes of the tree based on frequencies
    priority_queue<HuffmanNode *, vector<HuffmanNode *>, compare>
        minHeap;

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            minHeap.push(new HuffmanNode((unsigned char)(i), freq[i]));
        }
    }

    if (minHeap.empty()) {
        return nullptr;
    }

    while (minHeap.size() != 1) {
        // Removes 2 nodes with the lowest frequency from the minHeap
        HuffmanNode *left = minHeap.top();
        minHeap.pop();
        HuffmanNode *right = minHeap.top();
        minHeap.pop();

        // Create a node to connect the left and right nodes, using the left and right sum as frequency
        unsigned int sum = left->freq + right->freq;
        minHeap.push(new HuffmanNode('$', sum, left, right));
    }

    // Return the reamaining node in the minHeap. This is the root node.
    return minHeap.top();
}

// Get the length for all symbols by traversing the tree
void getCodeLengths(HuffmanNode *node, int depth, vector<int> &codeLengths) {
    if (!node)
        return;

    // Leaf node
    if (!node->left && !node->right) {
        codeLengths[node->data] = depth;
        return;
    }

    // Recursively traverse both sides of the tree
    getCodeLengths(node->left, depth + 1, codeLengths);
    getCodeLengths(node->right, depth + 1, codeLengths);
}

// Delete the Huffman tree
void freeHuffmanTree(HuffmanNode *node) {
    if (!node)
        return;
    freeHuffmanTree(node->left);
    freeHuffmanTree(node->right);
    delete node;
}

void buildCanonicalCodes(const vector<int> &codeLengths, vector<string> &codeBook) {
    vector<pair<unsigned char, int>> symbols;
    for (int i = 0; i < 256; i++) {
        if (codeLengths[i] > 0) {
            symbols.push_back({(unsigned char)i, codeLengths[i]});
        }
    }

    // override sort to sort by ASC
    sort(symbols.begin(), symbols.end(), [](auto &a, auto &b) {
        if (a.second == b.second) {
            return a.first < b.first;
        }
        return a.second < b.second;
    });

    unsigned int code = 0;
    int curLen = 0;

    int numSymbols = symbols.size();

    for (size_t i = 0; i < numSymbols; i++) {
        unsigned char sym = symbols[i].first;
        int length = symbols[i].second;

        if (length > curLen) {
            code <<= (length - curLen);
            curLen = length;
        }

        string bits;
        bits.reserve(length);
        unsigned int temp = code;

        for (int bitPos = length - 1; bitPos >= 0; --bitPos) {
            bits.insert(bits.begin(), (temp & 1) ? '1' : '0');
            temp >>= 1;
        }
        codeBook[sym] = bits;

        // increment for the next symbol
        code += 1;
    }
}

void checkHeader(string &header) {
    // Iterate the header to see what it looks like
    for (int i = 0; i < 256; ++i) {
        unsigned char codeLen = static_cast<unsigned char>(header[i]);
        if (codeLen > 0) {
            cout << "Symbol " << i
                 << " (char '" << static_cast<char>(i) << "'): "
                 << static_cast<int>(codeLen) << "\n";
        }
    }

    // Iterate and print in one line Hex
    cout << "Header (hex): ";
    for (unsigned char c : header) {
        // Print each byte as two hex digits
        cout << hex << setw(2) << setfill('0')
             << static_cast<int>(c);
    }

    cout << dec << endl;
}

pair<string, vector<string>> createCanonicalHuffman(const string &input) {
    // Count frequencies
    vector<unsigned> freq(256, 0);
    countFrequencies(input, freq);

    // Build Huffman tree
    HuffmanNode *root = buildTree(freq);
    if (!root) {
        // empty input
        return {"", vector<string>(256, "")};
    }

    // Get code lengths
    vector<int> codeLengths(256, 0);
    getCodeLengths(root, 0, codeLengths);

    // Build canonical code patterns
    vector<string> codebook(256, "");
    buildCanonicalCodes(codeLengths, codebook);

    // Construct a 256-byte header storing each symbol’s code length
    string header(256, '\0');
    for (int i = 0; i < 256; ++i) {
        unsigned char len = (unsigned char)min(codeLengths[i], 255);
        header[i] = (char)len;
    }

    // DEBUG -- to check header
    checkHeader(header);

    // Free tree
    freeHuffmanTree(root);
    return {header, codebook};
}

// Rebuild the canonical codebook from the stored header of code lengths
vector<string> rebuildCodebookFromHeader(const string &header) {
    if (header.size() != 256) {
        throw runtime_error("Invalid header size, must be 256 bytes");
    }
    // Convert header into codeLengths
    vector<int> codeLengths(256, 0);
    for (int i = 0; i < 256; ++i) {
        // read length as unsigned char
        codeLengths[i] = (unsigned char)header[i];
    }
    // Build canonical codes
    vector<string> codebook(256, "");
    buildCanonicalCodes(codeLengths, codebook);
    return codebook;
}

// Decompress using the bitStream and the canonical code lengths
string decompressData(const string &bitStream, const string &header) {
    // Rebuild the codebook from the header
    vector<string> codebook = rebuildCodebookFromHeader(header);

    // Create a reverse lookup: bitPattern -> symbol so we can decode quickly
    unordered_map<string, unsigned char> patternToSymbol;
    for (int i = 0; i < 256; ++i) {
        if (!codebook[i].empty()) {
            patternToSymbol[codebook[i]] = (unsigned char)i;
        }
    }

    // Iterate the bitStream to collect bits a matching code is found
    string decoded;
    string currentBits;
    for (char bit : bitStream) {
        currentBits.push_back(bit);
        auto it = patternToSymbol.find(currentBits);
        if (it != patternToSymbol.end()) {
            decoded.push_back((char)it->second);
            currentBits.clear();
        }
    }
    return decoded;
}

static void appendCode(vector<uint8_t> &outBytes, uint8_t &currentByte, int &bitPos, const string &code) {
    for (char bit : code) {
        if (bit == '1') {
            currentByte |= (1 << bitPos);
        }
        bitPos++;

        if (bitPos == 8) {
            outBytes.push_back(currentByte);
            currentByte = 0;
            bitPos = 0;
        }
    }
}

/**
 *  pair( vector of bytes, total bits used )
 */
pair<vector<uint8_t>, int>
compressDataBinary(const string &input, const vector<string> &codebook) {
    vector<uint8_t> outBytes;
    outBytes.reserve(input.size());

    uint8_t currentByte = 0;
    int bitPos = 0;
    int totalBits = 0;

    for (unsigned char c : input) {
        const string &code = codebook[c];
        appendCode(outBytes, currentByte, bitPos, code);
        totalBits += code.size();
    }

    // If partial byte
    if (bitPos > 0) {
        outBytes.push_back(currentByte);
    }

    return {outBytes, totalBits};
}

string decompressDataBinary(const vector<uint8_t> &compressedBytes,
                            int totalBits,
                            const string &header) {
    // Rebuild codebook
    vector<string> codebook = rebuildCodebookFromHeader(header);

    // Reverse lookup
    unordered_map<string, unsigned char> patternToSymbol;
    for (int i = 0; i < 256; ++i) {
        if (!codebook[i].empty()) {
            patternToSymbol[codebook[i]] = (unsigned char)i;
        }
    }

    string decoded;
    string currentBits;
    int bitsRead = 0;
    int numBytes = compressedBytes.size();

    for (int byteIndex = 0; byteIndex < numBytes; ++byteIndex) {
        uint8_t byteVal = compressedBytes[byteIndex];
        // up to 8 bits
        for (int b = 0; b < 8; b++) {
            if (bitsRead == totalBits) {
                break;
            }
            int bit = (byteVal >> b) & 1;
            currentBits.push_back(bit ? '1' : '0');

            auto it = patternToSymbol.find(currentBits);
            if (it != patternToSymbol.end()) {
                decoded.push_back((char)it->second);
                currentBits.clear();
            }

            bitsRead++;
        }
    }
    return decoded;
}

pair<string, string> huffmanCompress(const string &data) {
    // Build the canonical Huffman from data
    auto [header, codebook] = createCanonicalHuffman(data);

    if (header.empty()) {
        return {"", ""};
    }

    // Compress into binary
    auto [packedBytes, totalBits] = compressDataBinary(data, codebook);

    // Create the 'body' that includes an integer (4 bytes) for totalBits
    //    followed by the raw bytes
    string body;
    body.resize(sizeof(int)); // allocate space for 4 bytes
    // copy totalBits into the first 4 bytes
    memcpy(&body[0], &totalBits, sizeof(totalBits));

    // now append the compressed bytes
    body.insert(body.end(),
                reinterpret_cast<const char *>(packedBytes.data()),
                reinterpret_cast<const char *>(packedBytes.data() + packedBytes.size()));

    return {header, body};
}

// Decompress from 256-byte header + body
string huffmanDecompress(const string &header, const string &body) {
    if (header.size() != 256) {
        // Something's wrong
        cerr << "huffmanDecompress: Invalid header size.\n";
        return "";
    }
    if (body.size() < 4) {
        // Not even enough bytes to store totalBits
        cerr << "huffmanDecompress: Body too small.\n";
        return "";
    }

    // Extract totalBits from the first 4 bytes
    int totalBits = 0;
    memcpy(&totalBits, &body[0], sizeof(totalBits));

    // The rest of 'body' is the compressed bytes
    vector<uint8_t> packedBytes;
    packedBytes.resize(body.size() - 4);
    memcpy(packedBytes.data(), &body[4], body.size() - 4);

    // Use existing 'decompressDataBinary' to get the original string
    string decompressed = decompressDataBinary(packedBytes, totalBits, header);

    return decompressed;
}

int main() {
    // Original data
    string data = "ABABCCCDEEAAAAAABBBZZZZZ;;;";

    // Build canonical Huffman from data
    auto [header, codebook] = createCanonicalHuffman(data);

    // Compress data into binary
    auto [compressedBytes, totalBits] = compressDataBinary(data, codebook);
    cout << "\nCompressed size: " << compressedBytes.size()
         << " bytes, totalBits = " << totalBits << "\n";

    // Decompress
    string decompressed = decompressDataBinary(compressedBytes, totalBits, header);

    cout << "\nOriginal:     " << data << "\n";
    cout << "Decompressed: " << decompressed << "\n";
    return 0;
}
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

/**
 * @brief Build the Huffman Tree by repeatedly combining two nodes with the lowest frequncies
 * into a new until there is only one node left. The last node will become the root of the tree
 *
 * @param string of data to build the Huffman tree
 * @return Root of the newly built Huffman tree
 */
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

// /**
//  * @brief
//  *
//  * @param root The root of the Huffman tree
//  * @param str The original data
//  * @param huffmanCode Map to store the generated Huffman codes
//  */
// void printCodes(HuffmanNode *root, string str, unordered_map<char, string> &huffmanCode) {
//     if (!root)
//         return;

//     if (root->data != '$') {
//         huffmanCode[root->data] = str;
//     }

//     printCodes(root->left, str + "0", huffmanCode);
//     printCodes(root->right, str + "1", huffmanCode);
// }

// /**
//  * @brief Methods reads the original string and encode it using the
//  * "alphabet" generated by the Huffman tree
//  *
//  * @param data Original string to be proccessed
//  * @param huffmanCode Map storing the Huffman
//  * @return string String of the encoded data from the tree
//  */
// string encodeData(const string &data, vector<int> &freq) {
//     string encodedData;

//     for (unsigned char c : data) {
//         encodedData += freq[c];
//     }

//     return encodedData;
// }

// /**
//  * @brief Traverse the Huffman tree to create a string representation of the tree
//  *
//  * @param root Pointer to the root of the Huffman tree
//  * @param res String representation of the tree
//  */
// void treeAsString(HuffmanNode *root, string &res) {
//     if (!root)
//         return;

//     // If it is a leaf node, append '1' and the character to the string.
//     // If not a leaf node, append '0' to the string.
//     if (!root->left && !root->right) {
//         res += '1';
//         res += root->data;
//     } else {
//         res += '0';
//     }

//     // Recursively traverse the tree
//     treeAsString(root->left, res);
//     treeAsString(root->right, res);
// }

// /**
//  * @brief
//  *
//  * @param data
//  * @return pair<string, HuffmanNode *> Encoded data as a string and the root of the Huffman tree
//  */
// pair<string, HuffmanNode *> HuffmanCodes(const string &data) {
//     if (data.empty())
//         return {"", nullptr};

//     // Build the Huffman tree
//     HuffmanNode *root = buildTree(data);

//     unordered_map<char, string> huffmanCode;
//     printCodes(root, "", huffmanCode);

//     string encodedResult = encodeData(data, huffmanCode);

//     return {encodedResult, root};
// }

// string HuffmanDecompress(const string &encodedData, HuffmanNode *root) {
//     string decodedData;
//     HuffmanNode *currentNode = root;

//     for (char bit : encodedData) {
//         if (bit == '0') {
//             currentNode = currentNode->left;
//         } else if (bit == '1') {
//             currentNode = currentNode->right;
//         }
//         // Leaf node
//         if (!currentNode->left && !currentNode->right) {
//             decodedData += currentNode->data;
//             currentNode = root; // Go back to the root for the next character
//         }
//     }

//     return decodedData;
// }

// HuffmanNode *buildTreeFromString(const string &tree, int &pos) {
//     // If we've consumed all characters or gone out of bounds, return null
//     if (pos >= (int)tree.size()) {
//         return nullptr;
//     }

//     // Read the next token
//     char token = tree[pos++];

//     // If '1' => the next character is the data for a leaf
//     if (token == '1') {
//         if (pos >= (int)tree.size()) {
//             // String was malformed or incomplete
//             return nullptr;
//         }
//         char leafChar = tree[pos++];

//         // Create a leaf node (frequency can be 0 or anything if you just need structure)
//         HuffmanNode *leaf = new HuffmanNode(leafChar, 0);
//         return leaf;
//     } else {
//         // token == '0' => internal node
//         // Create an internal node with placeholder data
//         HuffmanNode *node = new HuffmanNode('$', 0);

//         node->left = buildTreeFromString(tree, pos);
//         node->right = buildTreeFromString(tree, pos);

//         return node;
//     }
// }

// HuffmanNode *stringToTree(const string &tree) {
//     int pos = 0;
//     return buildTreeFromString(tree, pos);
// }

// int main() {
//     string test = "AAAAAABCCCCCCDDEEEEE";

//     cout << test << endl;

//     pair<string, HuffmanNode *> p = HuffmanCodes(test);

//     string res = p.first;

//     cout << res << endl;

//     string decomp = HuffmanDecompress(res, p.second);

//     string tree;

//     treeAsString(p.second, tree);

//     cout << tree << endl;

//     cout << decomp << endl;

//     return 0;
// }

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

    // Free tree
    freeHuffmanTree(root);
    return {header, codebook};
}

// Compress using the codebook
string compressData(const string &input, const vector<string> &codebook) {
    string bitStream;
    for (unsigned char c : input) {
        bitStream += codebook[c];
    }
    return bitStream;
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
    // Now build canonical codes
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

    // Walk the bitStream, collecting bits until we match a code
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

int main() {
    // Original data
    string data = "ABABCCCDEEAAAAAABBBZZZZZ;;;";

    // Build canonical Huffman from data
    auto [header, codebook] = createCanonicalHuffman(data);

    // Compress data into a bitStream
    string compressedBits = compressData(data, codebook);

    //   - `header` (256 bytes) + `compressedBits` (variable length)
    cout << "Header size: " << header.size() << " bytes\n";
    cout << "Compressed bitstring: " << compressedBits << "\n\n";

    // Decompress data from (header + compressedBits)
    string decompressed = decompressData(compressedBits, header);

    cout << "Original  : " << data << "\n";
    cout << "Decompressed: " << decompressed << "\n";
    return 0;
}
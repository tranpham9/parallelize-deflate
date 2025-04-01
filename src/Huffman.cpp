#include <atomic>
#include <bitset>
#include <iostream>
#include <queue>
#include <sstream>
#include <stdint.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

class HuffmanNode {
public:
    char data;
    unsigned freq;
    HuffmanNode *left;
    HuffmanNode *right;
    bool isLeaf;

    // Constructor for leaf nodes
    HuffmanNode(char data, unsigned freq) {
        this->data = data;
        this->freq = freq;
        this->left = this->right = nullptr;
        this->isLeaf = true;
    }

    // Constructor for internal nodes
    HuffmanNode(unsigned freq, HuffmanNode *left, HuffmanNode *right) {
        this->data = '\0'; // not used
        this->freq = freq;
        this->left = left;
        this->right = right;
        this->isLeaf = false;
    }

    // Optional default constructor
    HuffmanNode() {
        this->data = '\0';
        this->freq = 0;
        this->left = this->right = nullptr;
        this->isLeaf = false;
    }
};

// Function object to be used as a custom comparator
struct compare {
    bool operator()(HuffmanNode *left, HuffmanNode *right) {
        return (left->freq > right->freq);
    }
};

string encode(const string &data, const unordered_map<char, string> &huffmanCode);
string decode(const string &encodedData, HuffmanNode *root);

/**
 * Converts a string of binary into ascii
 * String will be buffered by zero if data is not divisable by 8
 * @param
 * @return An ascii version of the binary. Note: It will always end with a null value
 */
string binaryToASCII(string &data) {
    if (data.empty()) {
        return nullptr;
    }

    // buffers the string of binary with 0's as needed
    if (data.length() % 8 != 0) {
        int bufferAmount = 8 - (data.length() % 8);
        data.append(bufferAmount, '0');
    }

    // converts the string of binary to ascii
    std::stringstream sstream(data);
    std::string output;
    while (sstream.good()) {
        std::bitset<8> bits;
        sstream >> bits;
        char c = char(bits.to_ulong());
        output += c;
    }

    return output;
}

/**
 * converts ascii to binary
 */
std::string asciiToBinary(const string &asciiString, uint64_t bitcount) {
    if (asciiString.empty()) {
        return nullptr;
    }

    // ascii to binary
    std::string binaryString = "";
    for (char c : asciiString) {
        binaryString += std::bitset<8>(c).to_string();
    }
    return binaryString.substr(0, bitcount);
}

/**
 * @brief Parallelized method to count the frequency of each character in the input string
 *
 * @param data Input string
 * @return unordered_map<char, unsigned> Map of character frequencies
 */
unordered_map<char, unsigned> countFreqParallel(const string &data) {
    const int numThreads = 8; // Get the number of available threads
    const size_t dataSize = data.size();
    const size_t chunkSize = (dataSize + numThreads - 1) / numThreads; // Divide data into chunks

    // Vector of atomic integers to store frequencies (256 for ASCII characters)
    vector<atomic<unsigned>> globalFreq(256);

    // Lambda function for each thread to process a chunk
    auto countChunk = [&](size_t start, size_t end) {
        vector<unsigned> localFreq(256, 0); // Local frequency map for this thread
        for (size_t i = start; i < end; ++i) {
            localFreq[static_cast<unsigned char>(data[i])]++;
        }
        // Merge local frequencies into the global atomic frequency map
        for (int i = 0; i < 256; ++i) {
            globalFreq[i] += localFreq[i];
        }
    };

    // Create threads to process chunks
    vector<thread> threads;
    for (int t = 0; t < numThreads; ++t) {
        size_t start = t * chunkSize;
        size_t end = min(start + chunkSize, dataSize);
        if (start < end) {
            threads.emplace_back(countChunk, start, end);
        }
    }

    for (auto &t : threads) {
        t.join();
    }

    // Convert the global frequency vector to an unordered_map
    unordered_map<char, unsigned> freqMap;
    for (int i = 0; i < 256; ++i) {
        if (globalFreq[i] > 0) {
            freqMap[static_cast<char>(i)] = globalFreq[i];
        }
    }

    return freqMap;
}

/**
 * @brief Build the Huffman Tree by repeatedly combining two nodes with the lowest frequncies
 * into a new until there is only one node left. The last node will become the root of the tree
 *
 * @param string of data to build the Huffman tree
 * @return Root of the newly built Huffman tree
 */
HuffmanNode *buildTree(string data) {
    // Use an unordered map to count the occurences of characters
    unordered_map<char, unsigned> freq = countFreqParallel(data);

    // Minheap to store the nodes of the tree based on frequencies
    priority_queue<HuffmanNode *, vector<HuffmanNode *>, compare> minHeap;

    // Create a new node for each character and add node to the minHeap
    for (auto &pair : freq) {
        minHeap.push(new HuffmanNode(pair.first, pair.second));
    }

    while (minHeap.size() != 1) {
        // Removes 2 nodes with the lowest frequency from the minHeap
        HuffmanNode *left = minHeap.top();
        minHeap.pop();
        HuffmanNode *right = minHeap.top();
        minHeap.pop();

        // Create a node to connect the left and right nodes, using the left and right sum as frequency
        int sum = left->freq + right->freq;
        minHeap.push(new HuffmanNode(sum, left, right));
    }

    // Return the reamaining node in the minHeap. This is the root node.
    return minHeap.top();
}

/**
 * @brief
 *
 * @param root The root of the Huffman tree
 * @param str The original data
 * @param huffmanCode Map to store the generated Huffman codes
 */
void printCodes(HuffmanNode *root, string str, unordered_map<char, string> &huffmanCode) {
    if (!root)
        return;

    if (root->isLeaf) {
        huffmanCode[root->data] = str;
    }

    printCodes(root->left, str + "0", huffmanCode);
    printCodes(root->right, str + "1", huffmanCode);
}

/**
 * @brief Methods reads the original string and encode it using the
 * "alphabet" generated by the Huffman tree
 *
 * @param data Original string to be proccessed
 * @param huffmanCode Map storing the Huffman
 * @return string String of the encoded data from the tree
 */
string encodeData(const string &data, const unordered_map<char, string> &huffmanCode) {
    string encodedData;
    for (char c : data) {
        if (huffmanCode.find(c) == huffmanCode.end()) {
            cerr << "Character not found in Huffman code map: '" << c << "' (" << int(c) << ")" << endl;
            return "";
        }
        encodedData += huffmanCode.at(c);
    }

    return encodedData;
}

/**
 * @brief Traverse the Huffman tree to create a string representation of the tree
 *
 * @param root Pointer to the root of the Huffman tree
 * @param res String representation of the tree
 */
void treeAsString(HuffmanNode *root, string &res) {
    if (!root) {

        return;
    }
    // If it is a leaf node, append '1' and the character to the string.
    // If not a leaf node, append '0' to the string.
    if (root->isLeaf) {
        res += '1';
        res += root->data;
    } else {
        res += '0';
    }

    // Recursively traverse the tree
    treeAsString(root->left, res);
    treeAsString(root->right, res);
}

HuffmanNode *stringAsTree(HuffmanNode *root, string &res, int &index) {
    if (index >= res.length())
        return nullptr;

    if (res[index] == '0') {
        index++;
        root = new HuffmanNode(); // internal node
        root->isLeaf = false;
        root->left = stringAsTree(root->left, res, index);
        root->right = stringAsTree(root->right, res, index);
    } else if (res[index] == '1') {
        index++;
        root = new HuffmanNode(res[index], 0); // leaf node
        root->isLeaf = true;
        index++;
    }

    return root;
}

void printTree(HuffmanNode *root) {
    if (root == nullptr) {
        return;
    }

    cout << root->data << " ";
    printTree(root->left);
    printTree(root->right);
}

/**
 * @brief
 *
 * @param data
 * @return pair<string, HuffmanNode *> Encoded data as a string and the root of the Huffman tree
 */
pair<string, HuffmanNode *> HuffmanCodes(const string &data) {
    if (data.empty())
        return {"", nullptr};

    // Build the Huffman tree
    HuffmanNode *root = buildTree(data);

    unordered_map<char, string> huffmanCode;
    printCodes(root, "", huffmanCode);

    string encodedResult = encodeData(data, huffmanCode);

    return {encodedResult, root};
}

string HuffmanDecompress(const string &encodedData, HuffmanNode *root) {
    string decodedData;
    HuffmanNode *currentNode = root;

    for (char bit : encodedData) {
        if (bit == '0') {
            currentNode = currentNode->left;
        } else if (bit == '1') {
            currentNode = currentNode->right;
        }
        // Leaf node
        if (!currentNode->left && !currentNode->right) {
            decodedData += currentNode->data;
            currentNode = root; // Go back to the root for the next character
        }
    }

    return decodedData;
}

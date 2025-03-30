#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

void writeFile(std::string text, std::string fileName)
{
    std::ofstream outfile(fileName);

    outfile << text;

    outfile.close();
}

/**
 * @brief Creates a file of random data of a given size
 * @param fileName The name of the file to be created
 * @param fileSize The size of the file to be created
 * @return 1 if successful, 0 if not
 */
int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        cout << "no input paramater was inputed" << endl;
        return EXIT_FAILURE;
    }

    // Check if the correct number of arguments is provided
    string fileName = argv[1];
    int fileSize = std::stoi(argv[2]);

    // seed the random number generator
    srand(static_cast<unsigned int>(time(0)));

    // fill the vector with random data
    vector<char> content(fileSize);
    for (int i = 0; i < fileSize; i++)
    {
        // Fill the vector with random data
        content[i] = static_cast<char>(rand() % 256); // Random byte
    }

    string s = string(content.begin(), content.end());
    writeFile(s, fileName);

    return 1;
}

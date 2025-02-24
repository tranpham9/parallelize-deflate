#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

using namespace std;

struct PatternResult
{
public:
    uint8_t matchLength;
    uint8_t index;

    PatternResult()
    {
        matchLength = 0;
        index = 0;
    }
};

class KMP_Search
{
private:
    void constructLps(string &txt, int patternStart, int patternEnd, vector<int> &lps)
    {

        // len stores the length of longest prefix which
        // is also a suffix for the previous index
        int len = patternStart;

        // lps[0] is always patternStart
        lps[0] = patternStart;

        int i = patternStart + 1;
        while (i < patternEnd)
        {
            // cout << i << ", " << len << endl;
            // cout << patternStart << " -> " << patternEnd << endl;

            // If characters match, increment the size of lps
            if (txt[i] == txt[len])
            {
                len++;
                lps[i - patternStart] = len;
                i++;
            }

            // If there is a mismatch
            else
            {
                if (len != patternStart)
                {

                    // Update len to the previous lps value
                    // to avoid reduntant comparisons
                    // cout << (len - patternStart - 1) << endl;
                    len = lps[len - patternStart - 1];
                }
                else
                {

                    // If no matching prefix found, set lps[i] to 0
                    lps[i - patternStart] = patternStart;
                    i++;
                }
            }
        }
    }

public:
    struct PatternResult search(string &txt, int patternStart, int patternEnd, int textStart, int textEnd)
    {
        struct PatternResult pattern;

        int n = textEnd;
        int m = patternEnd;
        if (n == textStart || m == patternStart)
        {
            return pattern;
        }

        vector<int> lps(m - patternStart);

        constructLps(txt, patternStart, patternEnd, lps);

        // Pointers i and j, for traversing
        // the text and pattern
        int i = textStart;    // traces the text
        int j = patternStart; // traces the pattern

        // cout << "==================================" << endl;
        // for (auto &&i : lps)
        // {
        //     cout << i << ", ";
        // }
        // cout << "\n";
        // for (auto &&i : lps)
        // {
        //     cout << (i-patternStart) << ", ";
        // }

        // std::string patternT = txt.substr(patternStart, patternEnd);
        // std::cout << "\npattern\t(" << patternT.length() << "): " << patternT << endl;
        // std::string compareWindow = txt.substr(textStart, textEnd);
        // std::cout << "window \t(" << compareWindow.length() << "): " << compareWindow << endl;

        while (i < n)
        {

            // If characters match, move both pointers forward
            // std::cout << txt[i] << "=" << pat[j] << endl;
            if (txt[i] == txt[j])
            {

                if (pattern.matchLength < (j - patternStart + 1))
                {
                    pattern.matchLength = j - patternStart + 1;
                    pattern.index = j - i;
                    // cout << "in:" << (int)pattern.index << ", " << (int)pattern.matchLength << " :: " << n << " + " << j << " - " << i << endl;
                }

                i++;
                j++;

                // If the entire pattern is matched
                // store the start index in result
                if (j == m)
                {
                    // cout << "end" << endl;
                    return pattern;
                }
            }

            // If there is a mismatch
            else
            {
                // Use lps value of previous index
                // to avoid redundant comparisons
                if (j != patternStart)
                    j = lps[j - patternStart - 1];
                else
                    i++;
            }
        }
        return pattern;
    }
};

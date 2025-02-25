#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

using namespace std;

struct PatternResult
{
public:
    size_t matchLength;
    size_t index;

    PatternResult()
    {
        matchLength = 0;
        index = 0;
    }
};

class KMP_Search
{
private:
    void constructLps(string &txt, size_t patternStart, size_t patternEnd, vector<size_t> &lps)
    {

        // len stores the length of longest prefix which
        // is also a suffix for the previous index
        size_t len = patternStart;

        // lps[0] is always patternStart
        lps[0] = patternStart;

        size_t i = patternStart + 1;
        while (i < patternEnd)
        {

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
    struct PatternResult search(string &txt, size_t patternStart, size_t patternEnd, size_t textStart, size_t textEnd)
    {
        struct PatternResult pattern;

        size_t n = textEnd;
        size_t m = patternEnd;
        if (n == textStart || m == patternStart)
        {
            return pattern;
        }

        vector<size_t> lps(m - patternStart);

        constructLps(txt, patternStart, patternEnd, lps);

        // Pointers i and j, for traversing
        // the text and pattern
        size_t i = textStart;    // traces the text
        size_t j = patternStart; // traces the pattern

        
        while (i < n)
        {

            // If characters match, move both pointers forward
            if (txt[i] == txt[j])
            {

                if (pattern.matchLength <= (j - patternStart + 1))
                {
                    pattern.matchLength = j - patternStart + 1;
                    pattern.index = j - i;
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

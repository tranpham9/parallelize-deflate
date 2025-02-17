#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct PatternResult
{
public:
    u_int8_t matchLength;
    u_int8_t index;

    PatternResult()
    {
        int matchLength = 0;
        int index = 0;
    }
};

class KMP_Search
{
private:
    void constructLps(string &pat, vector<int> &lps)
    {

        // len stores the length of longest prefix which
        // is also a suffix for the previous index
        int len = 0;

        // lps[0] is always 0
        lps[0] = 0;

        int i = 1;
        while (i < pat.length())
        {

            // If characters match, increment the size of lps
            if (pat[i] == pat[len])
            {
                len++;
                lps[i] = len;
                i++;
            }

            // If there is a mismatch
            else
            {
                if (len != 0)
                {

                    // Update len to the previous lps value
                    // to avoid reduntant comparisons
                    len = lps[len - 1];
                }
                else
                {

                    // If no matching prefix found, set lps[i] to 0
                    lps[i] = 0;
                    i++;
                }
            }
        }
    }

public:
    struct PatternResult search(string &pat, string &txt)
    {
        struct PatternResult pattern;

        int n = txt.length();
        int m = pat.length();
        if (n == 0 || m == 0)
        {
            return pattern;
        }

        vector<int> lps(m);

        constructLps(pat, lps);

        // Pointers i and j, for traversing
        // the text and pattern
        int i = 0;
        int j = 0;

        while (i < n)
        {

            // If characters match, move both pointers forward
            // std::cout << txt[i] << "=" << pat[j] << endl;
            if (txt[i] == pat[j])
            {

                if (pattern.matchLength <= j)
                {
                    pattern.matchLength = (j+1) > pattern.matchLength ? (j+1) : pattern.matchLength;
                    pattern.index = n + j - i;
                    // cout << "in:" << (int)pattern.index << ", " << (int)pattern.matchLength << " :: " << n << " - " << j<< " - " << i <<  endl;
                }

                i++;
                j++;

                // If the entire pattern is matched
                // store the start index in result
                if (j == m)
                {
                    return pattern;
                }
            }

            // If there is a mismatch
            else
            {
                // Use lps value of previous index
                // to avoid redundant comparisons
                if (j != 0)
                    j = lps[j - 1];
                else
                    i++;
                
            }
        }
        return pattern;
    }
};


// int main(){
//     KMP_Search kmp;

//     string pat = "bbabc";
//     string window = "aabc";

//     PatternResult result = kmp.search(pat, window);
//     cout << (int)result.index << ", " << (int)result.matchLength << endl;

//     return 1;
// }
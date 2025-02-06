#include <iostream>
#include <fstream>
#include <string>
#include <vector>



struct Token{
    std::byte length;
    char distance;
    char character;
};

class LZ77{

    private:

    std::string text;
    std::vector<Token> tokenText;
    std::string compressedText;


    void createTokenArray(){
        
    }

    void tokenArrayToString(){
        
        std::vector<char> buildText;

        for (auto && token : tokenText)
        {
            buildText.emplace_back(token.length);
            buildText.emplace_back(token.distance);
            buildText.emplace_back(token.character);
        }
        
        compressedText = std::string (buildText.begin(), buildText.end());
    }


    public:

    void compress(){

    }

    void decompress(){

    }

    std::string getText(){
        return text;
    }

    std::vector<Token>& getTokenText(){
        return tokenText;
    }

    std::string getCompressedText(){
        return compressedText;
    }

    std::string setText(std::string _text){
        text = _text;
    }

    std::string setTokenText(std::vector<Token>& _tokenText){
        tokenText = _tokenText;
    }

    std::string setCompressedText(std::string _compressedText){
        compressedText = _compressedText;
    }

};

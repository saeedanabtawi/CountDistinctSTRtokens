// Tokenizer.h

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>
using namespace std;
    
const std::string DEFAULT_DELIMITER = "\t\r\v\f, .";

class Tokenizer
{
    private:
        void skipDelimiter();                          // ignore leading delimiters
        bool isDelimiter(char c);                      // check if the current char is delimiter
        string buffer;                                 // input string
        
        string token;                                  // output string
        string delimiter;                              // delimiter string
        string::const_iterator currPos;                // string iterator pointing the current position

    public:
        Tokenizer();
        Tokenizer(const string& str, 
                  const string& delimiter=DEFAULT_DELIMITER);
        ~Tokenizer();

        // set string and delimiter
        void set(const string& str, 
                 const string& delimiter=DEFAULT_DELIMITER);

        void setString(const string& str);             // set source string only
        void setDelimiter(const string& delimiter);    // set delimiter string only

        void DisposeObject();
        
        string next();                            // return the next token, return "" if it ends
};

#endif // TOKENIZER_H

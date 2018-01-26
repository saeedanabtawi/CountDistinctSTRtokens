/* Tokenizer.cpp */
#include "Tokenizer.h" 
using namespace std;

/* constructor */
Tokenizer::Tokenizer() : buffer(""), token(""), delimiter(DEFAULT_DELIMITER)
{
    currPos = buffer.begin();
}

Tokenizer::Tokenizer(const string& str, 
                     const string& delimiter) : buffer(str), token(""), delimiter(delimiter)
{
    currPos = buffer.begin();
}

/* destructor */
Tokenizer::~Tokenizer()
{
}

/* reset string buffer, delimiter and the currsor position */
void Tokenizer::set(const string& str, 
                    const string& delimiter)
{
    this->buffer = str;
    this->delimiter = delimiter;
    this->currPos = buffer.begin();
}

void Tokenizer::setString(const string& str)
{
    this->buffer = str;
    this->currPos = buffer.begin();
}

void Tokenizer::setDelimiter(const string& delimiter)
{
    this->delimiter = delimiter;
    this->currPos = buffer.begin();
}

/*
    return the next token
    If cannot find a token anymore, return "".
*/
std::string Tokenizer::next()
{
    if(buffer.size() <= 0) return "";           // skip if buffer is empty

    token.clear();                              // reset token string

    this->skipDelimiter();                      // skip leading delimiters

    // append each char to token string until it meets delimiter
    while(currPos != buffer.end() && !isDelimiter(*currPos))
    {
        token += *currPos;
        ++currPos;
    }
    return token;
}

/* skip ang leading delimiters */
void Tokenizer::skipDelimiter()
{
    while(currPos != buffer.end() && isDelimiter(*currPos))
        ++currPos;
}

/* return true if the current character is delimiter */
bool Tokenizer::isDelimiter(char c)
{
    return (delimiter.find(c) != std::string::npos);
}
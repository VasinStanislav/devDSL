#ifndef LEXER_H
#define LEXER_H

#include <utility>
#include <regex>
#include <map>
#include <string>
#include <fstream>

class Lexer
{
        std::map<std::string, std::regex> lexems;
        //std::vector<std::string> keyWords;
    public:
        Lexer();
        std::string getTocken(std::string);
        std::multimap<std::string, std::string> * tokenize(std::ifstream *);
};

#endif
#ifndef LEXER_H
#define LEXER_H

#include <utility>
#include <regex>
#include <map>
#include <string>
#include <fstream>
#include <typeinfo>


typedef std::map<std::string, std::regex> M;
typedef std::multimap<std::string, std::string> MM;


class Lexer
{
        M lexems;
        M keyWords;

        template <typename T> std::string getTocken(std::string, T lexems);
    public:
        Lexer();
        MM * tokenize(std::ifstream *);
};

#endif
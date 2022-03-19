#ifndef LEXER_H
#define LEXER_H

#include <utility>
#include <regex>
#include <map>
#include <string>
#include <fstream>
#include <typeinfo>
#include <vector>
#include "Token.h"


typedef std::map<std::string, std::regex> M;
//typedef std::multimap<std::string, std::string> MM;
typedef std::vector<Token *> V;


class Lexer
{
        M lexems;
        M keyWords;

        template <typename T> std::string getTocken(std::string, T);
    public:
        Lexer();
        V * tokenize(std::ifstream *);
};

#endif
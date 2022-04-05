#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <utility>
#include <regex>
#include <map>
#include <string>
#include <fstream>
#include <typeinfo>
#include <vector>
#include "Token.h"


typedef std::map<std::string, std::regex> M;
typedef std::string string;
typedef std::pair<string, string> str_pair;
//typedef std::multimap<std::string, std::string> MM;
typedef std::vector<Token *> V;


class Lexer
{
        M lexems;
        M keyWords;

        template <typename T> string getTocken(string, T);
        str_pair nipOff(string);
    public:
        Lexer();
        V * tokenize(std::ifstream *);
};

#endif
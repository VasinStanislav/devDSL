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


typedef std::string string;
typedef std::regex   regex;

typedef std::map<string, regex>           M;
typedef std::pair<string, string>  str_pair;
typedef std::vector<Token *>              V;

class MyMap : public M
{
    public:
        void fill(string, regex);
};

class Lexer
{
        MyMap lexems;
        MyMap keyWords;

        template <typename T> string getTocken(string, T);
        str_pair nipOff(string);
    public:
        Lexer();
        V * tokenize(std::ifstream *);
};

#endif
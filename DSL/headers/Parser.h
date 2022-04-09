#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <iostream>
#include "Token.h"

typedef std::vector<Token *> V;
typedef V::iterator vecIt;

class Parser
{
        V tokenList;
        vecIt listIt;

        void handleError(const std::string &exception);
        void functionCheck();
        void argsCheck();
    public:
        Parser(V *);
        void syntaxCheck();

        ~Parser();
};


#endif
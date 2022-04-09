#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <iostream>
#include "Token.h"

typedef std::vector<Token *> V;
typedef V::iterator vecIt;

class Parser
{
        unsigned int curLineNum;
        V tokenList;
        vecIt listIt;

        void handleError(const std::string &exception);
        void functionCheck();
        void argsCheck();
        void blockCheck();
        void expressionCheck();
    public:
        Parser(V *);
        void syntaxCheck();

        ~Parser();
};


#endif
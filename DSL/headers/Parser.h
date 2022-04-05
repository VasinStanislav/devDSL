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

        void functionGrammar(Token *);
    public:
        Parser(V *);
        void grammarCheck();

        ~Parser();
};


#endif
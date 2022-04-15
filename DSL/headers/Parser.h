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

        void generateException(std::string, bool);
        void handleException(const std::string &);

        void functionCheck();
        void argsCheck();
        void blockCheck();
        void assignmentCheck();
        void expressionCheck();
    public:
        Parser(V *);
        void syntaxCheck();

        ~Parser();
};


#endif
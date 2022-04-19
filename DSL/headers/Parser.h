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

        void functionDefCheck();
        void functionCallCheck();
        void functionCheck();
        void argsCheck();
        void blockCheck();
        void assignmentCheck();
        void expressionCheck(bool inBrackets=false);
        void opReturnCheck();
    public:
        Parser(V *);
        void syntaxCheck();

        ~Parser();
};


#endif
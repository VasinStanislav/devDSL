#include "../headers/Parser.h"

Parser::Parser(V *tokenList) : tokenList(*tokenList),
                               listIt(this->tokenList.begin()){}

void Parser::grammarCheck()
{   
    while (listIt != tokenList.end())
    {
        if ((*listIt)->type == "FUNCTION")
        {
            this->functionGrammar(*listIt);
        }
        break;
    }
}

void Parser::functionGrammar(Token *curToken)
{
    this->listIt++;
}

Parser::~Parser()
{
    for (size_t it = 0; it < tokenList.size(); ++it)
    {
        delete tokenList[it];
    }

    tokenList.clear();
}
#include "../headers/Parser.h"

Parser::Parser(V *tokenList) : tokenList(*tokenList),
        listIt(this->tokenList.begin()), status(0){}


Parser::~Parser()
{
    for (size_t it = 0; it < tokenList.size(); ++it)
    {
        delete tokenList[it];
    }

    tokenList.clear();
}



void Parser::syntaxCheck()
{   
    while (listIt != tokenList.end())
    {
        if ((*listIt)->type == "FUNCTION")
        {
            this->functionCheck();
        }
        break;
    }
}

/* ----------------------EXPRESSIONS----------------------- */

void Parser::functionCheck()
{
    this->listIt++;
    try
    {
        if (listIt!=tokenList.end() and (*listIt)->type == "L_BRACKET")
        {
            listIt++;
        }
        else 
        {
            throw "( was expected; " + (*listIt)->type + " was provided";
        }

        if (listIt!=tokenList.end())
        {
            this->argsCheck();
            listIt++;

            if (listIt!=tokenList.end() and (*listIt)->type == "R_BRACKET")
            {
                listIt++;
            }
            else 
            {
                throw ") was expected; " + (*listIt)->type + " was provided";
            }
        }

    }
    catch(const std::string &exception)
    {
        std::cerr<<exception<<"\n";
        listIt = tokenList.end();
        status = -1;
        return;
    }
}

void Parser::argsCheck()
{
    try
    {
        if (listIt!=tokenList.end() and (*listIt)->type == "R_BRACKET")
        {
            listIt--;
        }
        else if (listIt!=tokenList.end() and ((*listIt)->type == "VARIABLE" or
                (*listIt)->type == "INTEGER" or (*listIt)->type == "STRING"))
        { 
            listIt++;

            while(listIt!=tokenList.end())
            {
                if ((*listIt)->type == "R_BRACKET") 
                {
                    listIt--;
                    break;
                }
                if ((*listIt)->type == "ARG_SEPARATOR")
                {
                    listIt++;
                    if ((*listIt)->type == "VARIABLE" or (*listIt)->type == "INTEGER" or
                        (*listIt)->type == "STRING")
                    {
                        listIt++;
                    }
                }
                else 
                {
                    throw ") or argument were expected; " + (*listIt)->type + " was provided";
                }
            }
        }
        else 
        {
            throw ") or argument were expected; " + (*listIt)->type + " was provided";
        }
    }
    catch(const std::string &exception)
    {
        std::cerr<<exception<<"\n";
        listIt = tokenList.end();
        status = -1;
        return;
    }
}
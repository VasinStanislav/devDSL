#include "../headers/Parser.h"

Parser::Parser(V *tokenList) : tokenList(*tokenList), listIt(this->tokenList.begin()){}

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
        try
        {
            if ((*listIt)->type == "FUNCTION")
            {
                this->functionCheck();
            }
        }
        catch(const std::string &exception)
        {
            this->handleError(exception);
            break;
        }
        break;
    }
}

void Parser::handleError(const std::string &exception)  
{ 
    std::cerr<<exception<<"; syntax analysis has been stopped\n"; 
}

/* ----------------------EXPRESSIONS----------------------- */

void Parser::functionCheck()
{
    this->listIt++;
    if (listIt!=tokenList.end() and (*listIt)->type == "L_BRACKET")
    {
        listIt++;
    }
    else 
    {
        throw "in line " + std::to_string((*listIt)->line) + ": ( was expected; " + 
        ((listIt==tokenList.end())? "nothing" : (*listIt)->type) + " was provided";
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
            throw "in line " + std::to_string((*listIt)->line) + ": ) was expected; " + 
            ((listIt==tokenList.end())? "nothing" : (*listIt)->type) + " was provided";
        }
    }
}

void Parser::argsCheck()
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
                else 
                {
                    throw "in line " + std::to_string((*listIt)->line) + 
                    ": argument was expected; " + ((listIt==tokenList.end())? "nothing" 
                    : (*listIt)->type) + " was provided";
                }
            }
            else 
            {
                throw "in line" + std::to_string((*listIt)->line) + 
                ": ) or argument were expected; " + ((listIt==tokenList.end())? "nothing" : 
                (*listIt)->type) + " was provided";
            }
        }
    }
    else 
    {
        throw "in line " + std::to_string((*listIt)->line) + ": ) or argument were expected; "
        + ((listIt==tokenList.end())? "nothing" : (*listIt)->type) + " was provided";
    }
}
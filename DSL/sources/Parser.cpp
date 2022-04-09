#include "../headers/Parser.h"

Parser::Parser(V *tokenList) : tokenList(*tokenList), listIt(this->tokenList.begin()),
                               curLineNum(1){}

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
            else if ((*listIt)->type == "L_BRACE")
            {
                this->blockCheck();
            }
            else if ((*listIt)->type == "VARIABLE")
            {
                this->expressionCheck();
            }
            else { break; }
        }
        catch(const std::string &exception)
        {
            this->handleError(exception);
            break;
        }
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
    if (listIt!=tokenList.end() and (*listIt)->type == "L_BRACKET") { listIt++; }
    else 
    {
        if (listIt!=tokenList.end()) { curLineNum = (*listIt)->line; }
        else { curLineNum = (*(tokenList.end()--))->line; }
        throw "in line " + std::to_string(curLineNum) + ": ( was expected; " + 
        ((listIt==tokenList.end())? "nothing" : (*listIt)->type) + " was provided";
    }

    if (listIt!=tokenList.end())
    {
        this->argsCheck();
        listIt++;
    }
}

void Parser::argsCheck()
{
    if (listIt!=tokenList.end() and (*listIt)->type == "R_BRACKET")
    {
        listIt++;
    }
    else if (listIt!=tokenList.end() and ((*listIt)->type == "VARIABLE" or
            (*listIt)->type == "INTEGER" or (*listIt)->type == "STRING"))
    { 
        listIt++;

        if (listIt==tokenList.end()) 
        {
            curLineNum = (*(tokenList.end()--))->line;
            throw "in line " + std::to_string(curLineNum) + ": ) or , was expected; " +
            "nothing was provided";
        }

        while (true)
        {
            if (listIt!=tokenList.end() and (*listIt)->type == "R_BRACKET") 
            {
                listIt++;
                break;
            }
            if (listIt!=tokenList.end() and (*listIt)->type == "ARG_SEPARATOR")
            {
                listIt++;
                if (listIt!=tokenList.end() and (*listIt)->type == "VARIABLE" or 
                    (*listIt)->type == "INTEGER" or (*listIt)->type == "STRING")
                {
                    listIt++;
                }
                else 
                {
                    if (listIt!=tokenList.end()) { curLineNum = (*listIt)->line; }
                    else { curLineNum = (*(tokenList.end()--))->line; }
                    throw "in line " + std::to_string(curLineNum) + 
                    ": argument was expected; " + ((listIt==tokenList.end())? "nothing" 
                    : (*listIt)->type) + " was provided";
                }
            }
            else 
            {
                if (listIt!=tokenList.end()) { curLineNum = (*listIt)->line; }
                else { curLineNum = (*(tokenList.end()--))->line; }
                throw "in line" + std::to_string(curLineNum) + 
                ": ) or argument were expected; " + ((listIt==tokenList.end())? "nothing" : 
                (*listIt)->type) + " was provided";
            }
        }
    }
    else 
    {
        if (listIt!=tokenList.end()) { curLineNum = (*listIt)->line; }
        else { curLineNum = (*(tokenList.end()--))->line; }
        throw "in line " + std::to_string(curLineNum) + ": ) or argument were expected; "
        + ((listIt==tokenList.end())? "nothing" : (*listIt)->type) + " was provided";
    }
}

void Parser::blockCheck()
{
    this->listIt++;
    if (listIt!=tokenList.end() and (*listIt)->type == "R_BRACE")
    { 
        listIt++; 
        return;
    }
    if (listIt==tokenList.end())
    {
        curLineNum = (*(tokenList.end()--))->line;
        throw "in line " + std::to_string(curLineNum) + ": } or expression were expected; "
        "nothing was provided";
    }

    while (true)
    {
        if (listIt!=tokenList.end() and (*listIt)->type == "R_BRACE")
        {
            listIt++;
            break;
        }
        else if (listIt==tokenList.end())
        {
            curLineNum = (*(tokenList.end()--))->line;
            throw "in line " + std::to_string(curLineNum) + ": } or expression were expected; "
            "nothing was provided";
        }
        else { this->expressionCheck(); }
    }
}

void Parser::expressionCheck()
{
    if (listIt!=tokenList.end() and (*listIt)->type == "RETURN") 
    {
        listIt++;
        return; 
    }
    if (listIt==tokenList.end() and (*listIt)->type != "VARIABLE")
    {
        if (listIt!=tokenList.end()) { curLineNum = (*listIt)->line; }
        else { curLineNum = (*(tokenList.end()--))->line; }
        throw "in line " + std::to_string(curLineNum) + ": lvalue was expected; "
        + ((listIt==tokenList.end())? "nothing" : (*listIt)->type) + " was provided";
    }
    else { listIt++; }

    if (listIt==tokenList.end() and (*listIt)->value != "=")
    {
        if (listIt!=tokenList.end()) { curLineNum = (*listIt)->line; }
        else { curLineNum = (*(tokenList.end()--))->line; }
        throw "in line " + std::to_string(curLineNum) + ": '=' was expected; "
        + ((listIt==tokenList.end())? "nothing" : (*listIt)->type) + " was provided";
    }
    else { listIt++; }

    if (listIt!=tokenList.end() and (*listIt)->type == "VARIABLE" or
        (*listIt)->type == "INTEGER" or (*listIt)->type == "STRING")
    {   
        curLineNum = (*listIt)->line;
        listIt++;
    }
    else 
    {
        if (listIt!=tokenList.end()) { curLineNum = (*listIt)->line; }
        else { curLineNum = (*(tokenList.end()--))->line; }
        throw "in line " + std::to_string(curLineNum) + ": rvalue was expected; "
        + ((listIt==tokenList.end())? "nothing" : (*listIt)->type) + " was provided";
    }

    while (true)
    {
        if (listIt==tokenList.end()) { break; }
        if ((*listIt)->type=="MATH_OPERATOR")
        {
            listIt++;
            if (listIt!=tokenList.end() and (*listIt)->type == "VARIABLE" or
                (*listIt)->type == "INTEGER" or (*listIt)->type == "STRING") 
            { 
                curLineNum = (*listIt)->line;
                listIt++; 
            }
            if (listIt!=tokenList.end() and (*listIt)->type!="MATH_OPERATOR")
            {
                if (curLineNum < (*listIt)->line)
                {
                    curLineNum = (*listIt)->line;
                    if ((*listIt)->type == "VARIABLE") { this->expressionCheck(); }
                }
                else 
                {
                    curLineNum = (*listIt)->line;
                    throw "in line " + std::to_string(curLineNum) + ": math. op or \\n were expected; " 
                    + (*listIt)->type + " was provided";
                }
            }
            else if (listIt!=tokenList.end() and (*listIt)->type=="MATH_OPERATOR")
            {
                if (listIt!=tokenList.end()) { curLineNum = (*listIt)->line; }
                else { curLineNum = (*(tokenList.end()--))->line; }
                throw "in line " + std::to_string(curLineNum) + ": rvalue was expected; "
                + ((listIt==tokenList.end())? "nothing" : (*listIt)->type) + " was provided";
            }
        }
    }

}
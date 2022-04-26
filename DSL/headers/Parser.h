#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <iostream>
#include <cstring>
#include "Token.h"

typedef std::vector<Token *> V;
typedef V::iterator VecIt;
typedef std::string string;

struct ParsingException : public std::exception
{
    string why;

    ParsingException(string why) : why(why){}

    const char *what() const throw()
    {
        return why.c_str();
    }

    const string getWhy() { return why; }
};

class Parser
{
        int curLineNum;
        const int lines;
        V tokenList;
        VecIt listIt;

        // exceptions processing
        string generateException(string, string);
        void generateFailure();

        // trying to parse something
        void expr();

        // expressions:
        void functionDef();
        void functionCall();
        void args();
        void block();
        void assignment();
        void arithmeticExpression();
        void conditionalExpression();
        void opReturn();
        void opBreak();
        void opContinue();
        void opIf();
        void opElif();
        void opElse();
        void opDoWhile();
        void opWhile();

        // athoms:
        void keyword(string);
        void function();
        void lBracket();
        void variable();
        void argsSeparator();
        void rBracket();
        void lBrace();
        void rBrace();
        void assignOp();
        void unaryOp();
        void logicalNegation();
        void value();
        void mathOp();
        void separator();
        void comprOp();
        void notEndLine();
    public:
        Parser(V *);
        ~Parser();

        int lang();
        string getCurType();
};

typedef std::pair<const char *,void(Parser::*)()> KeyFunc;
typedef std::vector<KeyFunc> FuncV;

#endif
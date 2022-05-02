#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <iostream>
#include <cstring>
#include <iomanip>
#include "Token.h"
#include "AST.h"

class Parser;

typedef std::vector<Token *>                               V;
typedef V::iterator                                        VecIt;
typedef std::string                                        string;
typedef std::pair<const char *,void(Parser::*)(ASTNode *)> KeyFunc;
typedef std::vector<KeyFunc>                               FuncV;

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
        unsigned int curLineNum;
        const unsigned int lines;
        V tokenList;
        VecIt listIt;
        AST tree;

        // exceptions processing
        string generateException(string, string);
        void generateFailure(ASTNode *);

        // trying to parse something
        void expr();

        // expressions:
        void functionDef(ASTNode *);
        void functionCall(ASTNode *);
        void args(ASTNode *);
        void block(ASTNode *);
        void assignment(ASTNode *);
        void allocation(ASTNode *);
        void arithmeticExpression(ASTNode *);
        void conditionalExpression(ASTNode *);
        void opReturn(ASTNode *);
        void opBreak(ASTNode *);
        void opContinue(ASTNode *);
        void opIf(ASTNode *);
        void opElif(ASTNode *);
        void opElse(ASTNode *);
        void opDoWhile(ASTNode *);
        void opWhile(ASTNode *);

        // athoms:
        void keyword(string, ASTNode *);
        void constructor(ASTNode *);
        void function(ASTNode *);
        void lBracket(ASTNode *);
        void variable(ASTNode *);
        void argsSeparator(ASTNode *);
        void rBracket(ASTNode *);
        void lBrace(ASTNode *);
        void rBrace(ASTNode *);
        void assignOp(ASTNode *);
        void unaryOp(ASTNode *);
        void logicalNegation(ASTNode *);
        void value(ASTNode *);
        void mathOp(ASTNode *);
        void separator(ASTNode *);
        void logicalOp(ASTNode *);
        void comprOp(ASTNode *);
        void endLine(ASTNode *);
        void notEndLine(ASTNode *);
    public:
        Parser(V *);
        ~Parser();

        int lang();
};

#endif
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
        AST * tree;

        // exceptions processing
        string generateException(string, string);
        void generateFailure(ASTNode *);

        // trying to parse something
        void expr();

        // non-terminals:
        void functionDef(ASTNode *);
        void functionCall(ASTNode *);
        void args(ASTNode *);
        void block(ASTNode *);
        void assignment(ASTNode *);
        void allocation(ASTNode *);
        void prefix(ASTNode **);
        void arithmeticExpression(ASTNode *);
        void postfix(ASTNode **, ASTNode **);
        void conditionalExpression(ASTNode *);
        void opReturn(ASTNode *);
        void opBreak(ASTNode *);
        void opContinue(ASTNode *);
        void opIf(ASTNode *);
        void opElif(ASTNode *);
        void opElse(ASTNode *);
        void opFor(ASTNode *);
        void opDoWhile(ASTNode *);
        void opWhile(ASTNode *);

        int arithmeticBrackets(ASTNode **, ASTNode **, ASTNode **, ASTNode **);
        int conditionalBrackets(ASTNode **, ASTNode **, ASTNode **);
        void checkConditional();
        void setOperator(ASTNode **, ASTNode **, ASTNode **);
        void addValue(ASTNode **, ASTNode **, ASTNode **);
        void addValue(ASTNode **, ASTNode **, ASTNode **, ASTNode **);
        void defineParent(ASTNode **, ASTNode **);

        // terminals:
        void keyword(string, ASTNode *);
        void constructor(ASTNode *);
        void function(ASTNode *);
        void lBracket();
        void variable(ASTNode *);
        void argsSeparator();
        void rBracket();
        void lBrace();
        void rBrace();
        void assignOp(ASTNode *);
        void unaryOp(ASTNode *);
        void logicalNegation(ASTNode *);
        void value(ASTNode *);
        void mathOp(ASTNode *);
        void separator();
        void logicalOp(ASTNode *);
        void comprOp(ASTNode *);
        void endLine();
        void notEndLine();
    public:
        Parser(V *);
        ~Parser();

        int lang();
        AST * getTree();
};

#endif
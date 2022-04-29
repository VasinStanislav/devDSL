#ifndef FUNCCALLEXPRAST_H
#define FUNCCALLEXPRAST_H

#include "ExprAST.h"
#include <string>
#include <vector>

class FuncCallExprAST : public ExprAST
{
        std::string call;
        std::vector<ExprAST *> args;
    public:
        FuncCallExprAST(const std::string &call, std::vector<ExprAST *> args)
            : call(call), args(args) {}
};

#endif
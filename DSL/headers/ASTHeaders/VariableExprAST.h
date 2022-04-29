#ifndef VARIABLEEXPRAST_H
#define VARIABLEEXPRAST_H

#include "ExprAST.h"
#include <string>

class VariableExprAST : public ExprAST 
{
        std::string name;
    public:
        VariableExprAST(const std::string &name) : name(name) {}
};

#endif
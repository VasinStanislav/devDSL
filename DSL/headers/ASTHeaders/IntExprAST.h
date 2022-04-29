#ifndef INTEXPRAST_H
#define INTEXPRAST_H

#include "ExprAST.h"

class IntExprAST : public ExprAST 
{
        int value;
    public:
        IntExprAST(int value) : value(value) {}
};

#endif
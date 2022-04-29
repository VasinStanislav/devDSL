#ifndef BINARYOPEXPRAST_H
#define BINARYOPEXPRAST_H

#include "ExprAST.h"

class BinaryOpExprAST : public ExprAST
{
        char op;
        ExprAST *lHS, *rHS;
    public:
        BinaryOpExprAST(ExprAST *lHS, ExprAST *rHS, char op)
            : lHS(lHS), rHS(rHS), op(op) {}
};

#endif
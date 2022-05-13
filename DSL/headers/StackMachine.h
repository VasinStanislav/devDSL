#ifndef STACKMACHINE_H
#define STACKMACHINE_H

#include <vector>
#include <stack>
#include "Token.h"
#include "AST.h"

typedef std::vector<Token>       Vector;
typedef Vector::iterator         VectorIt;
typedef std::stack<Token>        Stack;
typedef std::vector<Vector *>    Content;

class StackMachine
{
        Content    * heap;
        NodeVector * functions;
    public:
        ~StackMachine();
        void split(AST *);
        Content * getContentFromFunction(ASTNode *);

        Content * getHeap();
        NodeVector * getFunctions();

        Vector * toRPN(ASTNode *);
        void expressionToRPN(Stack *, ASTNode *);
};

#endif
#ifndef STACKMACHINE_H
#define STACKMACHINE_H

#include <vector>
#include <stack>
#include "Token.h"
#include "AST.h"

typedef std::vector<Token>          Vector;
typedef Vector::iterator            VectorIt;
typedef Vector::reverse_iterator    VectorRIt;
typedef std::vector<int>            VectorInt;
typedef VectorInt::iterator         VectorIntIt;
typedef VectorInt::reverse_iterator VectorIntRIt;
typedef std::stack<Token>           Stack;
typedef std::vector<Stack *>        Content;

class StackMachine
{
        Content    * heap;
        NodeVector * functions;
    public:
        StackMachine();
        ~StackMachine();
        void split(AST *);
        Content * getContentFromFunction(ASTNode *);

        Content * getHeap();
        NodeVector * getFunctions();

        Stack * toRPN(ASTNode *);
        void valueToRPN(Stack *, ASTNode *);
        void expressionToRPN(Stack *, ASTNode *);
        void opIfToRPN(Stack *, ASTNode *);

        void stackToVector(Vector *, Stack *);
        void vectorToStack(Stack *, Vector *);
};  

#endif
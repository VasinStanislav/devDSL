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

        Stack * toRPN(ASTNode *, bool inBlock = false, int shift = 0);
        void assignmentToRPN(Stack *, ASTNode *, int shift = 0);
        void expressionToRPN(Stack *, ASTNode *, int shift = 0);
        void opIfToRPN(Stack *, ASTNode *, int shift = 0);
        void opElifToRPN(Stack *, ASTNode *, int shift = 0);
        void opElseToRPN(Stack *, ASTNode *, int shift = 0);
        void opForToRPN(Stack *, ASTNode *, int shift = 0);
        void opWhileToRPN(Stack *, ASTNode *, int shift = 0);
        void opDoWhileToRPN(Stack *, ASTNode *, int shift = 0);
        void calleeToRPN(Stack *, ASTNode *, int shift = 0);
        void blockToRPN(Stack *, ASTNode *, int shift = 0);
        void opReturnToRPN(Stack *, ASTNode *, int shift = 0);

        void stackToVector(Vector *, Stack *);
        void vectorToStack(Stack *, Vector *);
};  

#endif
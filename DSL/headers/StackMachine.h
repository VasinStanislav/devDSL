#ifndef STACKMACHINE_H
#define STACKMACHINE_H

#include <vector>
#include <stack>
#include "Token.h"
#include "AST.h"

typedef std::vector<Token>       Vector;
typedef Vector::iterator         VectorIt;
typedef Vector::reverse_iterator VectorRIt;
typedef std::vector<int>         VectorInt;
typedef VectorInt::iterator      VectorIntIt;
typedef std::stack<Token>        Stack;
typedef std::vector<Stack *>     Content;

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
        void expressionToRPN(Stack *, ASTNode *);
        void opIfToRPN(Stack *, ASTNode *);

        void stackToVector(Vector *, Stack *);
        void vectorToStack(Stack *, Vector *);
};

class JumpCounter
{
        static bool      isRunning;
        static VectorInt jumps;
    public:
        static void runCounter();
        static void addJump(int);
        static VectorInt stopCounter();
};   

#endif
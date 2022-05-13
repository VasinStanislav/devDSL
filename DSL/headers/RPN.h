#ifndef RPN_H
#define RPN_H

#include <vector>
#include <stack>
#include "Token.h"
#include "AST.h"

typedef std::vector<Token *> V;
typedef V::iterator          VecIt;
typedef std::stack<Token *>  Stack;

class RPN
{
        Stack * stack;
        V     * heap;
    public:
        void toRPN(AST *);
        Stack * epxressionToRPN(ASTNode *);
        
};

#endif
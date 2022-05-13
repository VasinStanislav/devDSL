#include "../headers/StackMachine.h"

StackMachine::~StackMachine()
{
    for (size_t it = 0; it < heap->size(); ++it) { delete (*heap)[it]; }
    heap->clear();
    delete heap;
}

void StackMachine::split(AST * tree)
{
    NodeVector * exprs = tree->getChildrenPtr();

    for (auto it = exprs->begin(); it != exprs->end(); it++)
    {
        auto expr = (*it)->getLastChild();
        if (expr->getLabel().value == "def")
        {
            this->functions->push_back(expr);
        }
        else 
        {
            auto line = this->toRPN(expr);
            this->heap->push_back(line);
        }
    }
}

Vector * StackMachine::toRPN(ASTNode *nodePtr)
{
    if (nodePtr->getLabel().type == "ASSIGN_OPERATOR")
    {
        ASTNode *parentPtr = nodePtr->getLastChild();
        Stack *stack = new Stack();
        
        this->expressionToRPN(stack, parentPtr);

        Vector *rpn = new Vector();
        while (!stack->empty())
        {
            auto el = stack->top();
            stack->pop();
            rpn->push_back(el);
        }
        rpn->push_back((*nodePtr->getChildrenPtr()->begin())->getLabel());
        rpn->push_back(nodePtr->getLabel());

        return rpn;
    }

    return nullptr;
}

void StackMachine::expressionToRPN(Stack *stack, ASTNode *node)
{
    stack->push(node->getLabel());

    NodeVecRevIt rIt = node->getChildrenPtr()->rbegin();
    for (; rIt != node->getChildrenPtr()->rend(); rIt++)
    {
        this->expressionToRPN(stack, (*rIt));
    }
}
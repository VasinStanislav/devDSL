#include "../headers/StackMachine.h"

StackMachine::StackMachine() : heap(nullptr), functions(nullptr) {}

StackMachine::~StackMachine()
{
    /*for (size_t it = 0; it < heap->size(); ++it) { delete (*heap)[it]; }
    heap->clear();
    delete heap;*/
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

Stack * StackMachine::toRPN(ASTNode *nodePtr)
{
    if (nodePtr->getLabel().type == "ASSIGN_OPERATOR")
    {
        ASTNode *parentPtr = nodePtr->getLastChild();
        Stack *stack = new Stack();
        
        stack->push(nodePtr->getLabel());
        stack->push((*nodePtr->getChildrenPtr()->begin())->getLabel());
        this->expressionToRPN(stack, parentPtr);

        return stack;
    }
    else if (nodePtr->getLabel().type == "IF_KW")
    {
        NodeVector *opIfChildren = nodePtr->getChildrenPtr();
        Vector *buff = new Vector();

        JumpCounter::runCounter();

        // adding condition of operator if to the buffer
        NodeVecIt it = opIfChildren->begin();
        ASTNode *condition = *it;
        Stack *conditionStack = new Stack();
        this->expressionToRPN(conditionStack, condition);
        this->stackToVector(buff, conditionStack);
        delete conditionStack;
        conditionStack = nullptr;

        // adding p!F to the buffer
        unsigned int line = buff->back().line;
        Token pF = {"p!F", "pFalse", line};
        buff->push_back(pF);

        // handling block of operator if
        it++;
        Stack *blockStack = this->toRPN((*it)->getLastChild());
        if (blockStack) 
        { 
            this->stackToVector(buff, blockStack); 
            delete blockStack;
            blockStack = nullptr;
        }

        int pos = buff->size();
        JumpCounter::addJump(pos);
        
        for (it++; it!=opIfChildren->end(); it++)
        {
            Stack *other = this->toRPN(*it);
            if (other)
            {
                this->stackToVector(buff, other);
                delete other;
                other = nullptr;
            }

            int pos = buff->size();
            JumpCounter::addJump(pos);
        }

        // adding ┴ to the buffer
        line = buff->back().line;
        Token end = {"┴", "end", line};
        buff->push_back(end);

        VectorInt vInt = JumpCounter::stopCounter();
        VectorIntIt vIntIt = vInt.begin();
        for (auto &el : *buff)
        {
            if (el.type == "pFalse") 
            { 
                string pos = std::to_string(*vIntIt);
                el.value.replace(0, 1, pos);
                vIntIt++;
            }
        }

        Stack *stack = new Stack();
        this->vectorToStack(stack, buff);
        buff->clear();

        delete buff;
        return stack;
    }
    else if (nodePtr->getLabel().type == "ELIF_KW")
    {
        NodeVector *opIfChildren = nodePtr->getChildrenPtr();
        Vector *buff = new Vector();

        // adding condition of operator elif to the buffer
        NodeVecIt it = opIfChildren->begin();
        ASTNode *condition = *it;
        Stack *conditionStack = new Stack();
        this->expressionToRPN(conditionStack, condition);
        this->stackToVector(buff, conditionStack);
        delete conditionStack;
        conditionStack = nullptr;

        // adding p!F to the buffer
        unsigned int line = buff->back().line;
        Token pF = {"p!F", "pFalse", line};
        buff->push_back(pF);

        // handling block of operator if
        it++;
        Stack *blockStack = this->toRPN((*it)->getLastChild());
        if (blockStack) 
        { 
            this->stackToVector(buff, blockStack); 
            delete blockStack;
            blockStack = nullptr;
        }

        Stack *stack = new Stack();
        this->vectorToStack(stack, buff);
        buff->clear();
        delete buff;
        return stack;
    }
    else if (nodePtr->getLabel().type == "ELSE_KW")
    {
        NodeVector *opIfChildren = nodePtr->getChildrenPtr();
        Vector *buff = new Vector();

        // handling block of operator if
        NodeVecIt it = opIfChildren->begin();
        Stack *blockStack = this->toRPN((*it)->getLastChild());
        if (blockStack) 
        { 
            this->stackToVector(buff, blockStack); 
            delete blockStack;
            blockStack = nullptr;
        }

        Stack *stack = new Stack();
        this->vectorToStack(stack, buff);
        buff->clear();
        delete buff;
        return stack;
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

void StackMachine::opIfToRPN(Stack *stack, ASTNode *node)
{

}

void StackMachine::stackToVector(Vector *buff, Stack *stack)
{
    while(!stack->empty())
    {
        auto el = stack->top();
        stack->pop();
        buff->push_back(el);
    }
}

void StackMachine::vectorToStack(Stack *stack, Vector *buff)
{
    VectorRIt rIt = buff->rbegin();
    for (; rIt!=buff->rend(); rIt++)
    {
        stack->push(*rIt);
    }
}

/*-------------------------------------------JUMP COUNTER---------------------------------------------*/

bool      JumpCounter::isRunning = false;
VectorInt JumpCounter::jumps;

void JumpCounter::runCounter() { isRunning = true; }

void JumpCounter::addJump(int pos) {  if (isRunning){ jumps.push_back(pos); }  }

VectorInt JumpCounter::stopCounter() 
{
    isRunning = false;
    VectorInt oldJumps = jumps;
    jumps.clear();
    return oldJumps;
}
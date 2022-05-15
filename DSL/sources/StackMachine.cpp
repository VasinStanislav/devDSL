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

        VectorInt jumps;

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
        Stack *blockStack = this->toRPN(*it);
        if (blockStack) 
        { 
            this->stackToVector(buff, blockStack); 
            delete blockStack;
            blockStack = nullptr;
        }

        int pos = buff->size();
        jumps.push_back(pos);
        
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
            jumps.push_back(pos);
        }

        // adding ┴ to the buffer
        line = buff->back().line;
        Token end = {"┴", "end", line};
        buff->push_back(end);

        VectorIntIt jumpIt = jumps.begin();
        for (auto &el : *buff)
        {
            if (el.type == "pFalse") 
            { 
                string pos = std::to_string(*jumpIt);
                el.value.replace(0, 1, pos);
                jumpIt++;
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

        // handling block of operator elif
        it++;
        Stack *blockStack = this->toRPN(*it);
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

        // handling block of operator else
        NodeVecIt it = opIfChildren->begin();
        Stack *blockStack = this->toRPN(*it);
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
    else if (nodePtr->getLabel().type == "FOR_KW")
    {
        NodeVector *opForChildren = nodePtr->getChildrenPtr();
        Vector *buff = new Vector();

        VectorInt jumps;

        // handling initial assignment of operator for
        NodeVecIt it = opForChildren->begin();
        Stack *initialAssignmentStack = this->toRPN(*it);
        if (initialAssignmentStack)
        { 
            this->stackToVector(buff, initialAssignmentStack); 
            delete initialAssignmentStack;
            initialAssignmentStack = nullptr;
        }

        int pos = buff->size();
        int continuePos = pos;
        jumps.push_back(pos);

        // adding condition of operator for to the buffer
        it++;
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

        // handling block of operator for
        ASTNode *block = nodePtr->getLastChild();
        Stack *blockStack = this->toRPN(block);
        if (blockStack) 
        { 
            this->stackToVector(buff, blockStack); 
            delete blockStack;
            blockStack = nullptr;
        }

        // handling last operation of body of operator for
        it++;
        Stack *lastOperation = this->toRPN(*it);
        if (lastOperation)
        { 
            this->stackToVector(buff, lastOperation); 
            delete lastOperation;
            lastOperation = nullptr;
        }

        // adding p! to the buffer
        line = buff->back().line;
        Token p = {"p!", "p", line};
        buff->push_back(p);

        pos = buff->size();
        jumps.push_back(pos);

        // adding ┴ to the buffer
        line = buff->back().line;
        Token end = {"┴", "end", line};
        buff->push_back(end);

        VectorIntRIt jumpsIt = jumps.rbegin();
        for (auto &el : *buff)
        {
            if ((el.type == "pFalse" or el.type == "p") and el.value[0] == 'p') 
            { 
                string pos = std::to_string(*jumpsIt);
                el.value.replace(0, 1, pos);
                jumpsIt++;
            }
            else if (el.type == "pBreak")
            {
                string pos = std::to_string(*jumpsIt);
                el.value.replace(0, 1, pos);
                el.line = line;
            }
            else if (el.type == "pContinue")
            {
                string pos = std::to_string(continuePos);
                el.value.replace(0, 1, pos);
                el.line = line;
            }
            line = el.line;
        }

        Stack *stack = new Stack();
        this->vectorToStack(stack, buff);
        buff->clear();
        delete buff;
        return stack;
    }
    else if (nodePtr->getLabel().type == "WHILE_KW")
    {
        NodeVector *opWhileChildren = nodePtr->getChildrenPtr();
        Vector *buff = new Vector();

        VectorInt jumps;

        int pos = buff->size();
        jumps.push_back(pos);

        // adding condition of operator while to the buffer
        NodeVecIt it = opWhileChildren->begin();
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

        // handling block of operator while
        ASTNode *block = nodePtr->getLastChild();
        Stack *blockStack = this->toRPN(block);
        if (blockStack) 
        { 
            this->stackToVector(buff, blockStack); 
            delete blockStack;
            blockStack = nullptr;
        }

        // adding p! to the buffer
        line = buff->back().line;
        Token p = {"p!", "p", line};
        buff->push_back(p);

        pos = buff->size();
        jumps.push_back(pos);

        // adding ┴ to the buffer
        line = buff->back().line;
        Token end = {"┴", "end", line};
        buff->push_back(end);

        VectorIntRIt jumpsIt = jumps.rbegin();
        for (auto &el : *buff)
        {
            if ((el.type == "pFalse" or el.type == "p") and el.value[0] == 'p') 
            { 
                string pos = std::to_string(*jumpsIt);
                el.value.replace(0, 1, pos);
                jumpsIt++;
            }
            else if (el.type == "pBreak")
            {
                string pos = std::to_string(*jumpsIt);
                el.value.replace(0, 1, pos);
                el.line = line;
            }
            else if (el.type == "pContinue")
            {
                string pos = std::to_string(0);
                el.value.replace(0, 1, pos);
                el.line = line;
            }
            line = el.line;
        }

        Stack *stack = new Stack();
        this->vectorToStack(stack, buff);
        buff->clear();
        delete buff;
        return stack;
    }
    else if (nodePtr->getLabel().type == "DO_KW")
    {
        NodeVector *opWhileChildren = nodePtr->getChildrenPtr();
        Vector *buff = new Vector();

        VectorInt jumps;

        NodeVecIt it = opWhileChildren->begin();

        // handling block of operator do
        Stack *blockStack = this->toRPN(*it);
        if (blockStack) 
        { 
            this->stackToVector(buff, blockStack); 
            delete blockStack;
            blockStack = nullptr;
        }

        // adding condition of operator while to the buffer
        it++;
        ASTNode *condition = *it;
        Stack *conditionStack = new Stack();
        this->expressionToRPN(conditionStack, condition);
        this->stackToVector(buff, conditionStack);
        delete conditionStack;
        conditionStack = nullptr;

        // adding p! to the buffer
        unsigned int line = buff->back().line;
        Token p = {"p!F", "pFalse", line};
        buff->push_back(p);

        int pos = buff->size();
        jumps.push_back(pos);

        // adding ┴ to the buffer
        line = buff->back().line;
        Token end = {"┴", "end", line};
        buff->push_back(end);

        VectorIntIt jumpsIt = jumps.begin();
        for (auto &el : *buff)
        {
            if ((el.type == "pFalse" or el.type == "p") and el.value[0] == 'p') 
            { 
                string pos = std::to_string(*jumpsIt);
                el.value.replace(0, 1, pos);
                jumpsIt++;
            }
            else if (el.type == "pBreak")
            {
                string pos = std::to_string(*jumpsIt);
                el.value.replace(0, 1, pos);
                el.line = line;
            }
            else if (el.type == "pContinue")
            {
                string pos = std::to_string(0);
                el.value.replace(0, 1, pos);
                el.line = line;
            }
            line = el.line;
        }

        Stack *stack = new Stack();
        this->vectorToStack(stack, buff);
        buff->clear();
        delete buff;
        return stack;
    }
    else if (nodePtr->getLabel().value == "callee")
    {
        NodeVector * calleeChildren = nodePtr->getChildrenPtr();
        Vector *buff = new Vector();
        
        // adding function name to the buffer
        NodeVecIt it = calleeChildren->begin(); 
        buff->push_back((*it)->getLabel());

        // adding function args to the buffer
        for (it++; it!=calleeChildren->end(); it++)
        {
            Stack *argument = new Stack(); 
            this->expressionToRPN(argument, *it);
            if (!argument->empty()) { this->stackToVector(buff, argument); }
            delete argument;
        }

        // adding callee operation to the buffer
        buff->push_back(nodePtr->getLabel());

        Stack *stack = new Stack();
        this->vectorToStack(stack, buff);
        buff->clear();
        delete buff;
        return stack;
    }
    else if (nodePtr->getLabel().value == "block")
    {
        NodeVector *blockChildren = nodePtr->getChildrenPtr();
        NodeVecIt it = blockChildren->begin();

        Vector *buff = new Vector();

        VectorInt jumps;

        // adding content of block to the buffer
        bool wasBreakKW = false;
        for (; it!=blockChildren->end(); it++)
        {
            Stack *stack = this->toRPN(*it);
            if (stack)
            {
                if (stack->top().type == "p") { wasBreakKW = true; }
                this->stackToVector(buff, stack);
                delete stack;
            }
        }

        if (wasBreakKW) 
        { 
            int pos = buff->size();
            jumps.push_back(pos); 
        }

        if (!jumps.empty())
        {
            VectorIntIt jumpsIt = jumps.begin();
            for (auto &el : *buff)
            {
                if ((el.type == "pFalse" or el.type == "p") and el.value[0] == 'p') 
                { 
                    string pos = std::to_string(*jumpsIt);
                    el.value.replace(0, 1, pos);
                    jumpsIt++;
                }
            }
        }

        Stack *stack = new Stack();
        this->vectorToStack(stack, buff);
        buff->clear();
        delete buff;
        return stack;
    }
    else if (nodePtr->getLabel().type == "RETURN_KW")
    {
        Vector *buff = new Vector();

        // adding value to the buffer
        ASTNode *expression = nodePtr->getLastChild();
        if (expression)
        {
            Stack *stack = new Stack();
            this->expressionToRPN(stack, expression);
            if (!stack->empty())
            {
                this->stackToVector(buff, stack);
                delete stack;
            }
        }

        buff->push_back(nodePtr->getLabel());

        Stack *stack = new Stack();
        this->vectorToStack(stack, buff);
        buff->clear();
        delete buff;
        return stack;
    }
    else if (nodePtr->getLabel().type == "BREAK_KW")
    {
        Stack *stack = new Stack();
        stack->push({"p!", "pBreak", 0});
        return stack;
    }
    else if (nodePtr->getLabel().type == "CONTINUE_KW")
    {
        Stack *stack = new Stack();
        stack->push({"p!", "pContinue", 0});
        return stack;
    }

    return nullptr;
}

void StackMachine::valueToRPN(Stack *stack, ASTNode *node)
{

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

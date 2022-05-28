#include "../headers/StackMachine.h"

StackMachine::StackMachine() : heap(new Content()), functions(new NodeVector()) {}

StackMachine::~StackMachine()
{
    /*for (size_t it = 0; it < heap->size(); ++it) { delete (*heap)[it]; }
    heap->clear();
    delete heap;*/
}

Content * StackMachine::getHeap() { return heap; }

NodeVector * StackMachine::getFunctions() { return functions; }

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

Content * StackMachine::getContentFromFunction(ASTNode * def)
{
    Content * funcContent = new Content();
    ASTNode * funcBlock = (*def->getChildrenPtr()->begin())->getLastChild();
    NodeVector * blockChildren = funcBlock->getChildrenPtr();
    NodeVecIt blockChildrenIt = blockChildren->begin();
    for (; blockChildrenIt != blockChildren->end(); blockChildrenIt++)
    {
        funcContent->push_back(this->toRPN(*blockChildrenIt));
    }
    return funcContent;
}

Stack * StackMachine::toRPN(ASTNode *nodePtr, bool inBlock, int shift)
{
    Stack *stack = new Stack();

    if (!inBlock)
    {
        // adding ┴ to the buffer
        Token end = {"┴", "end", 0};
        stack->push(end);
    }

    if (nodePtr->getLabel().type == "ASSIGN_OPERATOR")   { this->assignmentToRPN(stack, nodePtr, shift); }
    else if (nodePtr->getLabel().type  == "IF_KW")       { this->opIfToRPN(stack, nodePtr, shift); }
    else if (nodePtr->getLabel().type  == "FOR_KW")      { this->opForToRPN(stack, nodePtr, shift); }
    else if (nodePtr->getLabel().type  == "WHILE_KW")    { this->opWhileToRPN(stack, nodePtr, shift); }
    else if (nodePtr->getLabel().type  == "DO_KW")       { this->opDoWhileToRPN(stack, nodePtr, shift); }
    else if (nodePtr->getLabel().value == "callee")      { this->calleeToRPN(stack, nodePtr, shift); }
    else if (nodePtr->getLabel().type  == "RETURN_KW")   { this->opReturnToRPN(stack, nodePtr, shift); }
    else if (nodePtr->getLabel().type  == "BREAK_KW")    { stack->push({"p!", "pBreak", 0}); }
    else if (nodePtr->getLabel().type  == "CONTINUE_KW") { stack->push({"p!", "pContinue", 0}); }

    if (stack->empty())
    {
        delete stack;
        stack = nullptr;
    }
    return stack;
}

/*------------------------------------------NON-TERMINALS TO RPN--------------------------------------*/

void StackMachine::assignmentToRPN(Stack *stack, ASTNode *node, int shift)
{
    ASTNode *parentPtr = node->getLastChild();
        
    if (stack->size() == 1) { stack->top().line = node->getLabel().line; }

    stack->push(node->getLabel());
    stack->push((*node->getChildrenPtr()->begin())->getLabel());
    this->expressionToRPN(stack, parentPtr, shift);
}

void StackMachine::expressionToRPN(Stack *stack, ASTNode *node, int shift)
{
    stack->push(node->getLabel());

    NodeVecRevIt rIt = node->getChildrenPtr()->rbegin();
    for (; rIt != node->getChildrenPtr()->rend(); rIt++)
    {
        this->expressionToRPN(stack, (*rIt));
    }
}

void StackMachine::opIfToRPN(Stack *stack, ASTNode *node, int shift)
{
    NodeVector *opIfChildren = node->getChildrenPtr();
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
    Stack *blockStack = new Stack();
    this->blockToRPN(blockStack, *it, buff->size()+shift);
    if (!blockStack->empty()) { this->stackToVector(buff, blockStack); }
    delete blockStack;
    blockStack = nullptr;

    int pos = buff->size();
    jumps.push_back(pos+shift);
        
    for (it++; it!=opIfChildren->end(); it++)
    {
        Stack *other = new Stack();

        if ((*it)->getLabel().type == "ELIF_KW") { this->opElifToRPN(other, *it, shift); }
        else { this->opElseToRPN(other, *it, shift); }

        if (!other->empty())
        {
            this->stackToVector(buff, other);
            delete other;
            other = nullptr;
        }

        int pos = buff->size();
        jumps.push_back(pos+shift);
    }

    VectorIntIt jumpIt = jumps.begin();
    for (auto &el : *buff)
    {
        if (el.value == "p!F")
        { 
            string pos = std::to_string(*jumpIt);
            el.value.replace(0, 1, pos);
            jumpIt++;
        }
    }

    if (stack->size() == 1) { stack->top().line = buff->back().line; }

    this->vectorToStack(stack, buff);
    buff->clear();

    delete buff;
}

void StackMachine::opElifToRPN(Stack *stack, ASTNode *node, int shift)
{
    NodeVector *opIfChildren = node->getChildrenPtr();
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

    this->vectorToStack(stack, buff);
    buff->clear();

    delete buff;
}

void StackMachine::opElseToRPN(Stack *stack, ASTNode *node, int shift)
{
    NodeVector *opIfChildren = node->getChildrenPtr();
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

    this->vectorToStack(stack, buff);
    buff->clear();
    delete buff;
}

void StackMachine::opForToRPN(Stack *stack, ASTNode *node, int shift)
{
    NodeVector *opForChildren = node->getChildrenPtr();
    Vector *buff = new Vector();

    VectorInt jumps;

    // handling initial assignment of operator for
    NodeVecIt it = opForChildren->begin();
    Stack *initialAssignmentStack = new Stack();
    this->assignmentToRPN(initialAssignmentStack, *it, buff->size()+shift);
    if (!initialAssignmentStack->empty()) { this->stackToVector(buff, initialAssignmentStack); }
    delete initialAssignmentStack;
    initialAssignmentStack = nullptr;

    int pos = buff->size();
    int continuePos = pos + shift;
    jumps.push_back(continuePos);

    // adding condition of operator for to the buffer
    it++;
    ASTNode *condition = *it;
    Stack *conditionStack = new Stack();
    this->expressionToRPN(conditionStack, condition, buff->size()+shift);
    this->stackToVector(buff, conditionStack);
    delete conditionStack;
    conditionStack = nullptr;

    // adding p!F to the buffer
    unsigned int line = buff->back().line;
    Token pF = {"p!F", "pFalse", line};
    buff->push_back(pF);

    // handling block of operator for
    ASTNode *block = node->getLastChild();
    Stack *blockStack = new Stack();
    this->blockToRPN(blockStack, block, buff->size()+shift);
    if (!blockStack->empty()) { this->stackToVector(buff, blockStack); }
    delete blockStack;
    blockStack = nullptr;

    // handling last operation of body of operator for
    it++;
    Stack *lastOperation = new Stack();
    this->assignmentToRPN(lastOperation, *it, buff->size()+shift);
    if (!lastOperation->empty()) { this->stackToVector(buff, lastOperation); }
    delete lastOperation;
    lastOperation = nullptr;

    // adding p! to the buffer
    line = buff->back().line;
    Token p = {"p!", "p", line};
    buff->push_back(p);

    pos = buff->size();
    jumps.push_back(pos+shift);

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

    if (stack->size() == 1) { stack->top().line = buff->back().line; }

    this->vectorToStack(stack, buff);
    buff->clear();
    delete buff;
}

void StackMachine::opWhileToRPN(Stack *stack, ASTNode *node, int shift)
{
    NodeVector *opWhileChildren = node->getChildrenPtr();
    Vector *buff = new Vector();

    VectorInt jumps;

    int pos = buff->size();
    jumps.push_back(pos+shift);

    // adding condition of operator while to the buffer
    NodeVecIt it = opWhileChildren->begin();
    ASTNode *condition = *it;
    Stack *conditionStack = new Stack();
    this->expressionToRPN(conditionStack, condition, buff->size()+shift);
    this->stackToVector(buff, conditionStack);
    delete conditionStack;
    conditionStack = nullptr;

    // adding p!F to the buffer
    unsigned int line = buff->back().line;
    Token pF = {"p!F", "pFalse", line};
    buff->push_back(pF);

    // handling block of operator while
    ASTNode *block = node->getLastChild();
    Stack *blockStack = new Stack();
    this->blockToRPN(blockStack, block, buff->size()+shift);
    if (!blockStack->empty()) {  this->stackToVector(buff, blockStack); }
    delete blockStack;
    blockStack = nullptr;

    // adding p! to the buffer
    line = buff->back().line;
    Token p = {"p!", "p", line};
    buff->push_back(p);

    pos = buff->size();
    jumps.push_back(pos+shift);

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
            string pos = std::to_string(shift);
            el.value.replace(0, 1, pos);
            el.line = line;
        }
        line = el.line;
    }

    if (stack->size() == 1) { stack->top().line = buff->back().line; }

    this->vectorToStack(stack, buff);
    buff->clear();
    delete buff;
}

void StackMachine::opDoWhileToRPN(Stack *stack, ASTNode *node, int shift)
{
    NodeVector *opWhileChildren = node->getChildrenPtr();
    Vector *buff = new Vector();

    VectorInt jumps;

    NodeVecIt it = opWhileChildren->begin();

    // handling block of operator do
    Stack *blockStack = new Stack();
    this->blockToRPN(blockStack, *it, buff->size()+shift);
    if (!blockStack->empty()) {  this->stackToVector(buff, blockStack); }
    delete blockStack;
    blockStack = nullptr;

    // adding condition of operator while to the buffer
    it++;
    ASTNode *condition = *it;
    Stack *conditionStack = new Stack();
    this->expressionToRPN(conditionStack, condition, buff->size()+shift);
    this->stackToVector(buff, conditionStack);
    delete conditionStack;
    conditionStack = nullptr;

    // adding p!F to the buffer
    unsigned int line = buff->back().line;
    Token pF = {"p!F", "pFalse", line};
    buff->push_back(pF);

    int pos = buff->size();
    jumps.push_back(pos+shift+1);

    // adding p! to the buffer
    Token p = {"p!", "p", line};
    buff->push_back(p);

    pos = shift;
    jumps.push_back(pos);

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
            string pos = std::to_string(shift);
            el.value.replace(0, 1, pos);
            el.line = line;
        }
        line = el.line;
    }

    if (stack->size() == 1) { stack->top().line = buff->back().line; }

    this->vectorToStack(stack, buff);
    buff->clear();
    delete buff;
}

void StackMachine::calleeToRPN(Stack *stack, ASTNode *node, int shift)
{
    NodeVector * calleeChildren = node->getChildrenPtr();
    Vector *buff = new Vector();
        
    // adding function name to the buffer
    NodeVecIt it = calleeChildren->begin(); 
    buff->push_back((*it)->getLabel());

    // adding function args to the buffer
    for (it++; it!=calleeChildren->end(); it++)
    {
        Stack *argument = new Stack(); 
        this->expressionToRPN(argument, *it, buff->size()+shift);
        if (!argument->empty()) { this->stackToVector(buff, argument); }
        delete argument;
    }

    if (stack->size() == 1) { stack->top().line = buff->back().line; }

    // adding callee operation to the buffer
    buff->push_back(node->getLabel());

    this->vectorToStack(stack, buff);
    buff->clear();
    delete buff;
}

void StackMachine::blockToRPN(Stack *stack, ASTNode *node, int shift)
{
    NodeVector *blockChildren = node->getChildrenPtr();
    NodeVecIt it = blockChildren->begin();

    Vector *buff = new Vector();

    VectorInt jumps;

    // adding content of block to the buffer
    bool wasBreakKW = false;
    for (; it!=blockChildren->end(); it++)
    {
        Stack *stack = this->toRPN(*it, true, buff->size()+shift);
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
        jumps.push_back(pos+shift); 
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

    this->vectorToStack(stack, buff);
    buff->clear();
    delete buff;
}

void StackMachine::opReturnToRPN(Stack *stack, ASTNode *node, int shift)
{
    Vector *buff = new Vector();

    // adding value to the buffer
    ASTNode *expression = node->getLastChild();
    if (expression)
    {
        Stack *stack = new Stack();
        this->expressionToRPN(stack, expression, buff->size()+shift);
        if (!stack->empty())
        {
            this->stackToVector(buff, stack);
            delete stack;
        }
    }

    buff->push_back(node->getLabel());

    this->vectorToStack(stack, buff);
    buff->clear();
    delete buff;
}

/*-----------------------------------------CONVERTING METHODS-----------------------------------------*/

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

/*--------------------------------------------SHOW RPN------------------------------------------------*/

void StackMachine::showRPN(Content *heap)
{
    std::cout<<"FUNCTION CONTAINS:\n";

    for (const auto &line : *heap)
    {
        struct hack : Stack
        {
            static Token item(Stack const& stack, size_t const index)
            {
                return (stack.*&hack::c)[index];
            }
        };

        Stack temporaryStack;
        for (size_t i = 0; i < line->size(); ++i)
        {
            temporaryStack.push(hack::item(*line, i));
        }
        while (!temporaryStack.empty())
        {
            std::cout<<temporaryStack.top().value<<" ";
            temporaryStack.pop();
        }
        std::cout<<"\n";
    }

    std::cout<<"END OF FUNCTION:\n";
}
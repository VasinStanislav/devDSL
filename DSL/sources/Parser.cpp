#include "../headers/Parser.h"

Parser::Parser(V *tokenList) : tokenList(*tokenList), listIt(this->tokenList.begin()),
    curLineNum(1), lines((tokenList->back())->line), tree({"lang", "", curLineNum}){}

Parser::~Parser()
{
    for (size_t it = 0; it < tokenList.size(); ++it) { delete tokenList[it]; }
    tokenList.clear();
}

// lang -> (expr)*

int Parser::lang()
{   
    std::cout<<std::setw(29)<<std::setfill('*')<<std::right<<"Par";
    std::cout<<std::setw(28)<<std::left<<"ser"<<"\n";

    std::cout<<"in lang!\n";

    try 
    {  
        while ((*listIt)->type != "EOF") { this->expr(); }  
    }
    catch (ParsingException &e) 
    { 
        this->tree.deleteLastChild();
        return 1; 
    }
    std::cout<<"parsed successfully!\n";

    std::cout<<std::setw(29)<<std::setfill('*')<<std::right<<"AS";
    std::cout<<std::setw(28)<<std::left<<"T"<<"\n";
    
    this->tree.showTree();
    return 0;
}

// expr -> (functionDef | functionCall | assignment | opIf | opWhile | opDoWhile){1}
 
void Parser::expr()
{
    ASTNode *expr = new ASTNode({"expr", "", curLineNum}, &tree);
    this->tree.addChild(expr);

    PredencyControl::initPriority();

    const FuncV expressions = 
    {
        {"functionDef", &Parser::functionDef}, {"functionCall", &Parser::functionCall},
        {"assignment", &Parser::assignment}, {"opIf", &Parser::opIf},
        {"opWhile", &Parser::opWhile}, {"opDoWhile", &Parser::opDoWhile},
        {"failure", &Parser::generateFailure}
    };

    VecIt fixedIt = listIt;

    std::cout<<"in expr!\n";
    for (KeyFunc keyFuncIt : expressions)
    {
        try
        {
            std::cout<<"trying "<<(&keyFuncIt)->first<<"\n";
            (this->*((&keyFuncIt)->second))(expr);
            std::cout<<"matched\n";
            fixedIt = listIt;
            break;
        }
        catch (ParsingException & e)
        {
            std::cerr<<e.what()<<"\n";
            if (e.getWhy() == "failed to parse") { throw e; }
            expr->deleteLastChild();
            listIt = fixedIt;
        }
    }
}

/*---------------------------------------------EXCEPTIONS---------------------------------------------*/

string Parser::generateException(string expected, string provided)
{
    curLineNum = (*listIt)->line;
    return "in line " + std::to_string(curLineNum) + "\nexpected: " + expected + "; " +
            provided + " was provided";
}

void Parser::generateFailure(ASTNode *) { throw ParsingException(std::string("failed to parse")); }

/*-------------------------------------------EXPRESSIONS----------------------------------------------*/

// functionDef -> def{1}function{1}lBracket{1}(variable{1}(argsSeparator{1}variable{1})*)?
// rBracket{1}block{1}

void Parser::functionDef(ASTNode *parentPtr)
{
    ASTNode * def = new ASTNode(parentPtr);
    parentPtr->addChild(def);
    this->keyword("def", def);

    ASTNode * function = new ASTNode(def);
    def->addChild(function);
    this->function(function);

    this->lBracket();

    ASTNode * argument = new ASTNode(def);
    try { this->variable(argument); def->addChild(argument); }
    catch (ParsingException & e) 
    { 
        this->rBracket();
        this->block(function);
        return;
    }
    while ((*listIt)->type != "EOF")
    {
        try { this->argsSeparator(); }
        catch (ParsingException &e) { break; }
        argument = new ASTNode(def);
        def->addChild(argument);
        this->variable(argument);
    }
    
    this->rBracket();
    this->block(function);
}

void Parser::defineParent(ASTNode **parentPtr, ASTNode **victimPtr)
{
    if ((*parentPtr)->getLabel().value=="value" or (*parentPtr)->getLabel().value=="callee")
    {
        (*parentPtr)->setLabel((*victimPtr)->getLabel());
        delete (*victimPtr);
        *victimPtr = *parentPtr;
    }
    else 
    {
        (*victimPtr)->setParentPtr(*parentPtr);
        (*parentPtr)->addChild(*victimPtr);
    }
}

// functionCall -> function{1}lBracket{1}(value{1}(argsSeparator{1}
// (arithmeticExpression{1}|conditionalExpression{1}))*)?rBracket{1}separator?

void Parser::functionCall(ASTNode *parentPtr)
{
    ASTNode * callee = new ASTNode({"callee", "", curLineNum});
    this->defineParent(&parentPtr, &callee);

    ASTNode * function = new ASTNode(callee);
    callee->addChild(function);
    this->function(function);

    this->lBracket();

    VecIt fixedIt = listIt;
    try { this->arithmeticExpression(callee); }
    catch (ParsingException &e) 
    {
        listIt = fixedIt;
        callee->deleteLastChild();
        try { this->conditionalExpression(callee); }
        catch (ParsingException &e) 
        { 
            listIt = fixedIt;
            callee->deleteLastChild(); 
            this->rBracket();
            return;
        }
    }
    while ((*listIt)->type != "EOF")
    {
        try { this->argsSeparator(); }
        catch (ParsingException &e) { break; }
        try { this->arithmeticExpression(callee); }
        catch (ParsingException &e) { callee->deleteLastChild(); this->conditionalExpression(callee); }
    }

    this->rBracket();

    try { this->separator(); }
    catch (ParsingException &e){}
}

// block -> lBrace{1}
// (functionDef|functionCall|assignment|opIf|opReturn|opWhile|opDoWhile|opBreak|opContinue)?
// rBrace{1}

void Parser::block(ASTNode *parentPtr)
{
    ASTNode * block = new ASTNode({"block", "", curLineNum}, parentPtr);
    parentPtr->addChild(block);

    PredencyControl::initPriority();

    this->lBrace();

    const FuncV blockExpressions
    {
        {"functionDef", &Parser::functionDef}, {"functionCall", &Parser::functionCall},
        {"assignment", &Parser::assignment}, {"opIf", &Parser::opIf},
        {"opReturn", &Parser::opReturn}, {"opWhile", &Parser::opWhile},
        {"opDoWhile", &Parser::opDoWhile}, {"opBreak", &Parser::opBreak}, 
        {"opContinue", &Parser::opContinue}, {"failure", &Parser::generateFailure}
    };
    VecIt fixedIt = listIt;    

    std::cout<<"in block!\n";
    while ((*listIt)->type != "EOF")
    {
        try
        {
            for (KeyFunc keyFuncIt : blockExpressions)
            {
                try 
                {
                    std::cout<<"trying "<<(&keyFuncIt)->first<<"\n";
                    (this->*((&keyFuncIt)->second))(block);
                    std::cout<<"matched\n";
                    fixedIt = listIt;
                    break;
                }
                catch (ParsingException & e) 
                {
                    std::cerr<<e.what()<<"\n";
                    if (e.getWhy() == "failed to parse") { throw e; }
                    block->deleteLastChild();
                    listIt = fixedIt;
                }
            }
        }
        catch (ParsingException & e) { this->rBrace(); break; }
    }
}

// assignment -> variable{1}assignOp{1}(arithmeticExpression|conditionalExpression){1}

void Parser::assignment(ASTNode *parentPtr)
{
    ASTNode * assignOp = new ASTNode(parentPtr);
    parentPtr->addChild(assignOp);

    ASTNode * variable = new ASTNode(assignOp);
    assignOp->addChild(variable);
    this->variable(variable);

    this->assignOp(assignOp);

    VecIt fixedIt = listIt;
    try { this->arithmeticExpression(assignOp); this->endLine(); }
    catch (ParsingException & e) 
    { 
        if (assignOp->getChildrenAmount()>1) { assignOp->deleteLastChild(); }
        listIt = fixedIt; 
        this->conditionalExpression(assignOp); 
    }
}

// allocation -> new{1}constructor{1}lBracket{1}(value{1}(argsSeparator{1}
// (arithmeticExpression{1}|conditionalExpression{1}))*)?rBracket{1}separator?

void Parser::allocation(ASTNode *parentPtr)
{
    ASTNode * new_ = new ASTNode();
    this->keyword("new", new_);
    this->defineParent(&parentPtr, &new_);

    ASTNode * constructor = new ASTNode(new_);
    new_->addChild(constructor);
    this->constructor(constructor);

    this->lBracket();

    VecIt fixedIt = listIt;
    try { this->arithmeticExpression(new_); }
    catch (ParsingException &e) 
    {
        listIt = fixedIt;
        new_->deleteLastChild();
        try { this->conditionalExpression(new_); }
        catch (ParsingException &e) 
        { 
            listIt = fixedIt;
            new_->deleteLastChild(); 
            this->rBracket();
            return;
        }
    }
    fixedIt = listIt;
    while ((*listIt)->type != "EOF")
    {
        try { this->argsSeparator(); }
        catch (ParsingException &e) { break; }
        try { this->arithmeticExpression(new_); }
        catch (ParsingException &e) 
        { 
            listIt = listIt; 
            new_->deleteLastChild(); 
            this->conditionalExpression(new_); 
        }
        fixedIt = listIt;
    }

    this->rBracket();

    try { this->separator(); }
    catch (ParsingException &e){}

}

void Parser::prefix(ASTNode **prefix)
{
    try 
    { 
        *prefix = new ASTNode();
        this->unaryOp(*prefix);
    }
    catch(ParsingException & e) { delete (*prefix); *prefix = nullptr; }
}

void Parser::postfix(ASTNode **postfix, ASTNode **prefix)
{
    try 
    { 
        *postfix = new ASTNode();
        this->unaryOp(*postfix);

        if (*prefix) 
        { 
            (*postfix)->setParentPtr(*prefix);
            (*prefix)->addChild(*postfix);
        }
    }
    catch(ParsingException & e) { delete (*postfix); *postfix = nullptr; }
}

void Parser::setOperator(ASTNode **parentPtr, ASTNode **opPtr, ASTNode **valuePtr)
{
    *valuePtr = (*parentPtr)->getLastChild();
    (*parentPtr)->deleteLastChild();

    (*valuePtr)->setParentPtr(*opPtr);
    (*opPtr)->addChild(*valuePtr);

    (*opPtr)->setParentPtr(*parentPtr);
    (*parentPtr)->addChild(*opPtr);

    PredencyControl::setCurPriority((*opPtr)->getLabel().value);
}

void Parser::addValue(ASTNode **parentPtr, ASTNode **logicalNegation, ASTNode **valuePtr)
{
    if (*logicalNegation)
    {
        (*logicalNegation)->setParentPtr(*parentPtr);
        (*parentPtr)->addChild(*logicalNegation);

        (*valuePtr)->setParentPtr(*logicalNegation);
        (*logicalNegation)->addChild(*valuePtr);
    }
    else 
    {
        (*valuePtr)->setParentPtr(*parentPtr);
        (*parentPtr)->addChild(*valuePtr); 
    }
}

void Parser::addValue(ASTNode **parentPtr, ASTNode **postfix, ASTNode **prefix, ASTNode **valuePtr)
{
    if (*prefix) 
    { 
        (*prefix)->setParentPtr(*parentPtr);
        (*parentPtr)->addChild(*prefix);

        if (*postfix)
        {
            (*valuePtr)->setParentPtr(*postfix);
            (*postfix)->addChild(*valuePtr);
        }
        else 
        {
            (*valuePtr)->setParentPtr(*prefix);
            (*prefix)->addChild(*valuePtr);
        }
    }
    else if (*postfix) 
    { 
        (*postfix)->setParentPtr(*parentPtr);
        (*parentPtr)->addChild(*postfix);
        (*valuePtr)->setParentPtr(*postfix);
        (*postfix)->addChild(*valuePtr);
    }
    else 
    { 
        (*valuePtr)->setParentPtr(*parentPtr);
        (*parentPtr)->addChild(*valuePtr); 
    }
}

int Parser::arithmeticBrackets(ASTNode **parentPtr, ASTNode **postfix, ASTNode **prefix, 
                                ASTNode **valuePtr)
{
    this->prefix(prefix);

    try { this->lBracket(); }
    catch (ParsingException & e) 
    { 
        if (*prefix) 
        {
            delete (*prefix);
            listIt--;
        }
        return 0; 
    }

    this->arithmeticExpression(*parentPtr);
    *valuePtr = (*parentPtr)->getLastChild();
    (*parentPtr)->deleteLastChild();

    this->rBracket();

    this->postfix(postfix, prefix);

    this->addValue(parentPtr, postfix, prefix, valuePtr);

    PredencyControl::setCurPriority(-1);

    return 1;
}

// arithmeticExpression -> ( (lBracket{1}arithmeticExpression{1}rBracket{1}mathOp{1})?notEndLine{1}
// prefix?value{1}postfix?((mathOp{1}arithmeticExpression{1})|(separator?)){1}

void Parser::arithmeticExpression(ASTNode *parentPtr)
{
    // (5) '-' 4 '*' 2
    ASTNode * valuePtr1 = nullptr, * valuePtr2 = nullptr, * opPtr = nullptr;
    ASTNode * prefix = nullptr, * postfix = nullptr;
    int brackets;

    PredencyControl::setCurPriority(parentPtr->getLabel().value);

    do 
    {
        try
        {
            brackets = this->arithmeticBrackets(&parentPtr, &postfix, &prefix, &valuePtr1);

            if (!brackets) { break; }

            try 
            { 
                opPtr = new ASTNode();
                this->mathOp(opPtr);
                this->setOperator(&parentPtr, &opPtr, &valuePtr1); 
            }
            catch (ParsingException & e) { delete opPtr; return; }
        }
        catch (ParsingException & e) { throw e; }
    } while (false);

    prefix   = nullptr;
    postfix  = nullptr;
    brackets = 0;

    if (opPtr) { parentPtr = opPtr; }

    opPtr = new ASTNode();

    valuePtr2 = new ASTNode({"value", "", curLineNum});

    this->notEndLine();

    do
    {
        try 
        { 
            brackets = this->arithmeticBrackets(&valuePtr2, &postfix, &prefix, &valuePtr1); 
            if (brackets) 
            { 
                valuePtr1 = valuePtr2->getLastChild();
                valuePtr2->deleteLastChild();

                valuePtr1->setParentPtr(nullptr);
                delete valuePtr2;

                valuePtr2 = valuePtr1;

                break; 
            }
        }
        catch (ParsingException & e){}

        this->prefix(&prefix);
        VecIt fixedIt = listIt;
        try { this->value(valuePtr2); }
        catch (ParsingException &e) 
        { 
            try { this->functionCall(valuePtr2); }
            catch (ParsingException &e) 
            { 
                listIt = fixedIt;
                valuePtr2->deleteLastChild();
                try {this->allocation(valuePtr2); }
                catch (ParsingException &e){ delete valuePtr2; throw e; } 
            } 
        }
        this->postfix(&postfix, &prefix);
    } while (false);

    try 
    { 
        this->mathOp(opPtr);

        if (PredencyControl::getPredency(opPtr->getLabel().value) > PredencyControl::getCurPriority()) 
        { 
            swap(parentPtr, opPtr); 
            this->addValue(&parentPtr, &postfix, &prefix, &valuePtr2);
        }
        else 
        { 
            parentPtr->addChild(opPtr); 
            opPtr->setParentPtr(parentPtr); 
            this->addValue(&opPtr, &postfix, &prefix, &valuePtr2);
        } 

        PredencyControl::setCurPriority(opPtr->getLabel().value);
    }
    catch (ParsingException & e)
    {
        delete opPtr;
        this->addValue(&parentPtr, &postfix, &prefix, &valuePtr2);
        try { this->separator(); }
        catch (ParsingException & e){}
        return;
    }
    this->arithmeticExpression(opPtr);
}

int Parser::conditionalBrackets(ASTNode **parentPtr, ASTNode **logicalNegation, ASTNode **valuePtr)
{
    try { *logicalNegation = new ASTNode(); this->logicalNegation(*logicalNegation); }
    catch (ParsingException & e) { delete (*logicalNegation); (*logicalNegation) = nullptr; }
    try { this->lBracket(); }
    catch (ParsingException & e) 
    { 
        if (*logicalNegation) 
        {
            delete (*logicalNegation);
            listIt--;
        }
        return 0; 
    }

    this->conditionalExpression(*parentPtr);
    *valuePtr = (*parentPtr)->getLastChild();

    if (*logicalNegation)
    {
        (*parentPtr)->deleteLastChild();

        (*logicalNegation)->setParentPtr(*parentPtr);
        (*parentPtr)->addChild(*logicalNegation);

        (*valuePtr)->setParentPtr(*logicalNegation);
        (*logicalNegation)->addChild(*valuePtr);
    }

    this->rBracket();

    PredencyControl::setCurPriority(-1);

    return 1;
}

// conditionalExpression -> ( lBracket{1}(conditionalExpression{1}rBracket{1}(comprOp{1}|logicalOp))?
// notEndLine{1}arithmeticExpression{1}(((comprOp{1}|logicalOp{1})conditionalExpression{1})|
// (separator?)){1}

void Parser::conditionalExpression(ASTNode *parentPtr)
{
    // (5) > 4 and 2 < 3
    ASTNode * valuePtr1 = nullptr, * valuePtr2 = nullptr, * opPtr = nullptr;
    ASTNode * logicalNegation = nullptr;
    int brackets;

    PredencyControl::setCurPriority(parentPtr->getLabel().value);

    do 
    {
        try
        {
            brackets = this->conditionalBrackets(&parentPtr, &logicalNegation, &valuePtr1);
            if (!brackets) { break; }

            try 
            { 
                opPtr = new ASTNode();
                this->comprOp(opPtr); 
                this->setOperator(&parentPtr, &opPtr, &valuePtr1);

                break; 
            }
            catch (ParsingException & e){}
            try 
            {
                this->logicalOp(opPtr);
                this->setOperator(&parentPtr, &opPtr, &valuePtr1); 
            }
            catch (ParsingException & e) { delete opPtr; return; }
        }
        catch (ParsingException & e) { throw e; }
    } while (false);

    if (opPtr) { parentPtr = opPtr; }


    opPtr = new ASTNode();

    valuePtr2 = new ASTNode({"value", "", curLineNum});

    this->notEndLine();

    do
    {
        try 
        { 
            brackets = this->conditionalBrackets(&valuePtr2, &logicalNegation, &valuePtr1);
            if (brackets) 
            { 
                valuePtr1 = valuePtr2->getLastChild();
                valuePtr2->deleteLastChild();

                valuePtr1->setParentPtr(nullptr);
                delete valuePtr2;

                valuePtr2 = valuePtr1;

                break; 
            }
        }
        catch (ParsingException & e){}

        try { logicalNegation = new ASTNode(); this->logicalNegation(logicalNegation); }
        catch (ParsingException & e) { delete logicalNegation; logicalNegation = nullptr; }
        VecIt fixedIt = listIt;
        try { this->value(valuePtr2); }
        catch (ParsingException &e) 
        { 
            try { this->functionCall(valuePtr2); }
            catch (ParsingException &e) 
            { 
                listIt = fixedIt;
                valuePtr2->deleteLastChild();
                try {this->allocation(valuePtr2); }
                catch (ParsingException &e){ delete valuePtr2; throw e; } 
            } 
        }
    } while (false);

    do
    {
        try 
        { 
            this->comprOp(opPtr); 
            
            if (PredencyControl::getPredency(opPtr->getLabel().value) >
                PredencyControl::getCurPriority()) 
            {
                swap(parentPtr, opPtr);
                this->addValue(&parentPtr, &logicalNegation, &valuePtr2);
            }
            else
            { 
                parentPtr->addChild(opPtr); 
                opPtr->setParentPtr(parentPtr); 
                this->addValue(&opPtr, &logicalNegation, &valuePtr2);
            } 

            PredencyControl::setCurPriority(opPtr->getLabel().value);

            break; 
        }
        catch (ParsingException & e){}
        try 
        { 
            this->logicalOp(opPtr); 
            
            if (PredencyControl::getPredency(opPtr->getLabel().value) >
                PredencyControl::getCurPriority()) 
            {
                swap(parentPtr, opPtr);
                this->addValue(&parentPtr, &logicalNegation, &valuePtr2);
            }
            else
            { 
                parentPtr->addChild(opPtr); 
                opPtr->setParentPtr(parentPtr); 
                this->addValue(&opPtr, &logicalNegation, &valuePtr2);
            } 

            PredencyControl::setCurPriority(opPtr->getLabel().value);

        }
        catch (ParsingException & e)
        {
            delete opPtr;
            this->addValue(&parentPtr, &logicalNegation, &valuePtr2);
            try { this->separator(); }
            catch (ParsingException & e){}
            return;
        }
    } while (false);

    this->conditionalExpression(opPtr);
}

// opReturn -> (return){1}(arithmeticExpression{1}|conditionalExpression{1}|allocation{1})?separator?

void Parser::opReturn(ASTNode *parentPtr)
{
    ASTNode * opReturn = new ASTNode({"opReturn", "", curLineNum}, parentPtr);
    parentPtr->addChild(opReturn);

    this->keyword("return", opReturn);

    VecIt fixedIt = listIt;
    try { this->arithmeticExpression(opReturn); this->endLine(); }
    catch (ParsingException & e) { listIt = fixedIt; this->conditionalExpression(opReturn); }
}

// opBreak -> (break){1}separator?

void Parser::opBreak(ASTNode *parentPtr)
{
    ASTNode * opBreak = new ASTNode({"opBreak", "", curLineNum}, parentPtr);
    parentPtr->addChild(opBreak);

    this->keyword("break", opBreak);

    try { this->separator(); }
    catch (ParsingException &e){}
}

// opContinue -> (continue){1}separator?

void Parser::opContinue(ASTNode *parentPtr)
{
    ASTNode * opContinue = new ASTNode({"opContinue", "", curLineNum}, parentPtr);
    parentPtr->addChild(opContinue);

    this->keyword("continue", opContinue);

    try { this->separator(); }
    catch (ParsingException &e){}
}

// opIf -> (if){1}lBracket{1}conditionalExpression{1}rBracket{1}block{1}opElif?opElse?

void Parser::opIf(ASTNode *parentPtr)
{
    ASTNode * opIf = new ASTNode({"opIf", "", curLineNum}, parentPtr);
    parentPtr->addChild(opIf);

    this->keyword("if", opIf);
    this->lBracket();
    this->conditionalExpression(opIf);
    this->rBracket();
    this->block(opIf);

    VecIt fixedIt = listIt;
    try { this->opElif(opIf); }
    catch (ParsingException & e) { opIf->deleteLastChild(); listIt = fixedIt; }

    try { this->opElse(opIf); }
    catch (ParsingException & e) { opIf->deleteLastChild(); listIt = fixedIt; }
}

// opElif -> (elif){1}lBracket{1}conditionalExpression{1}rBracket{1}block{1}opElif?opElse?

void Parser::opElif(ASTNode *parentPtr)
{
    ASTNode * opElif = new ASTNode({"opElif", "", curLineNum}, parentPtr);
    parentPtr->addChild(opElif);

    this->keyword("elif",opElif);
    this->lBracket();
    this->conditionalExpression(opElif);
    this->rBracket();
    this->block(opElif);

    VecIt fixedIt = listIt;
    try { this->opElif(opElif); }
    catch (ParsingException & e) { opElif->deleteLastChild(); listIt = fixedIt; }

    fixedIt = listIt;
    try { this->opElse(opElif); }
    catch (ParsingException & e) { opElif->deleteLastChild(); listIt = fixedIt; }
}

// opElse -> (else){1}block{1}

void Parser::opElse(ASTNode *parentPtr)
{
    ASTNode * opElse = new ASTNode({"opElse", "", curLineNum}, parentPtr);
    parentPtr->addChild(opElse);

    this->keyword("else", opElse);
    this->block(opElse);
}

// opDoWhile -> (do){1}block{1}(while){1}lBracket{1}conditionalExpression{1}rBracket{1}separator?

void Parser::opDoWhile(ASTNode *parentPtr)
{
    ASTNode * opDoWhile = new ASTNode({"opDoWhile", "", curLineNum}, parentPtr);
    parentPtr->addChild(opDoWhile);

    this->keyword("do", opDoWhile);
    this->block(opDoWhile);
    this->keyword("while", opDoWhile);
    this->lBracket();
    this->conditionalExpression(opDoWhile);
    this->rBracket();
    try { this->separator(); }
    catch(ParsingException & e){}
}

// opWhile -> (while){1}lBracket{1}conditionalExpression{1}rBracket{1}block{1}

void Parser::opWhile(ASTNode *parentPtr)
{
    ASTNode * opWhile = new ASTNode({"opWhile", "", curLineNum}, parentPtr);
    parentPtr->addChild(opWhile);

    this->keyword("while", opWhile);
    this->lBracket();
    this->conditionalExpression(opWhile);
    this->rBracket();
    this->block(opWhile);
}

/*-------------------------------------------------ATHOMS---------------------------------------------*/

void Parser::keyword(string concrete, ASTNode *ptr)
{
    if (std::strcmp((*listIt)->value.c_str(), concrete.c_str()) != 0) 
    {
        throw ParsingException(generateException(concrete, (*listIt)->type)); 
    }
    else {  ptr->setLabel(**listIt); listIt++; }
}

void Parser::constructor(ASTNode *ptr)
{
    if ((*listIt)->type != "CONSTRUCTOR")
    {
        throw ParsingException(generateException(string("constructor"), (*listIt)->type));
    }
    else { ptr->setLabel(**listIt); listIt++; }
}

void Parser::function(ASTNode *ptr)
{
    if ((*listIt)->type != "FUNCTION")
    {
        throw ParsingException(generateException(string("function"), (*listIt)->type));
    }
    else { ptr->setLabel(**listIt); listIt++; }
}

void Parser::lBracket()
{
    if ((*listIt)->type != "L_BRACKET")
    {
        throw ParsingException(generateException(std::string("'('"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::variable(ASTNode *ptr)
{
    if ((*listIt)->type != "VARIABLE")
    {
        throw ParsingException(generateException(std::string("variable"), (*listIt)->type));
    }
    else { ptr->setLabel(**listIt); listIt++; }   
}

void Parser::argsSeparator()
{
    if ((*listIt)->type != "ARG_SEPARATOR")
    {
        throw ParsingException(generateException(std::string("','"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::rBracket()
{
    if ((*listIt)->type != "R_BRACKET")
    {
        throw ParsingException(generateException(std::string("')'"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::lBrace()
{
    if ((*listIt)->type != "L_BRACE")
    {
        throw ParsingException(generateException(std::string("'{'"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::rBrace()
{
    if ((*listIt)->type != "R_BRACE")
    {
        throw ParsingException(generateException(std::string("'}'"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::assignOp(ASTNode *ptr)
{
    if ((*listIt)->type != "ASSIGN_OPERATOR")
    {
        throw ParsingException(generateException(std::string("'='"), (*listIt)->type));
    }
    else { ptr->setLabel(**listIt); listIt++; }   
}

void Parser::unaryOp(ASTNode *ptr)
{
    if ((*listIt)->type != "UNARY_OPERATOR")
    {
        throw ParsingException(generateException(std::string("unary operator"), (*listIt)->type));
    }
    else { ptr->setLabel(**listIt); listIt++; }
}

void Parser::logicalNegation(ASTNode *ptr)
{
    if ((*listIt)->type != "LOGICAL_NEGATION")
    {
        throw ParsingException(generateException(std::string("'!'"), (*listIt)->type));
    }
    else { ptr->setLabel(**listIt); listIt++; }
}

void Parser::value(ASTNode *ptr)
{
    if ((*listIt)->type != "INTEGER" and (*listIt)->type != "STRING" and
        (*listIt)->type != "VARIABLE" and (*listIt)->type != "FLOAT" and
        (*listIt)->type != "CONSTANT_KW")
    {
        throw ParsingException(generateException(std::string("value"), (*listIt)->type));
    }
    else { ptr->setLabel(**listIt); listIt++; }
}

void Parser::mathOp(ASTNode *ptr)
{
    if ((*listIt)->type != "MATH_OPERATOR")
    {
        throw ParsingException(generateException(std::string("math. operator"), (*listIt)->type));
    }
    else { ptr->setLabel(**listIt); listIt++; }
}

void Parser::separator()
{
    if ((*listIt)->type != "SEPARATOR")
    {
        throw ParsingException(generateException(std::string("';'"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::comprOp(ASTNode *ptr)
{
    if ((*listIt)->type != "COMPRASION_OPERATOR")
    {
        throw ParsingException(generateException(std::string("compr. operator"), (*listIt)->type));
    }
    else { ptr->setLabel(**listIt); listIt++; }   
}

void Parser::logicalOp(ASTNode *ptr)
{
    if ((*listIt)->value != "and" and (*listIt)->value != "or" and
        (*listIt)->value != "nand" and (*listIt)->value != "nor")
    {
        throw ParsingException(generateException(std::string("logical operator"), (*listIt)->type));
    }
    else { ptr->setLabel(**listIt); listIt++; }   
}

void Parser::endLine()
{
    listIt--;
    int prevTokenLine  = (*listIt)->line;
    string probablySep = (*listIt)->type;

    listIt++;
        curLineNum    = (*listIt)->line;

    if (curLineNum == prevTokenLine or probablySep == "SEPARATOR")
    {
        throw ParsingException(generateException(std::string("'\\n' or ';'"), (*listIt)->type));
    }
}

void Parser::notEndLine()
{
    listIt--;
    int prevTokenLine = (*listIt)->line;

    listIt++;
        curLineNum    = (*listIt)->line;

    if (curLineNum > prevTokenLine)
    {
        throw ParsingException(generateException(std::string("value"), (*listIt)->type));
    }
}
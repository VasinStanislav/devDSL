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
    ASTNode * functionDef = new ASTNode({"functionDef", "", curLineNum}, parentPtr);
    parentPtr->addChild(functionDef);

    unsigned int fixedLineNum = this->curLineNum;

    this->keyword("def", functionDef);
    this->function(functionDef);
    this->lBracket(functionDef);

    try { this->variable(functionDef); }
    catch (ParsingException & e) 
    { 
        this->rBracket(functionDef);
        this->block(functionDef);
        return;
    }
    while ((*listIt)->type != "EOF")
    {
        try { this->argsSeparator(functionDef); }
        catch (ParsingException &e) { break; }
        this->variable(functionDef);
    }
    
    this->rBracket(functionDef);
    this->block(functionDef);
}

// functionCall -> function{1}lBracket{1}(value{1}(argsSeparator{1}
// (arithmeticExpression{1}|conditionalExpression{1}))*)?rBracket{1}separator?

void Parser::functionCall(ASTNode *parentPtr)
{
    ASTNode * functionCall = new ASTNode({"functionCall", "", curLineNum}, parentPtr);
    parentPtr->addChild(functionCall);

    this->function(functionCall);
    this->lBracket(functionCall);

    try { this->value(functionCall); }
    catch (ParsingException &e) 
    { 
        this->rBracket(functionCall);
        return;
    }
    while ((*listIt)->type != "EOF")
    {
        try { this->argsSeparator(functionCall); }
        catch (ParsingException &e) { break; }
        try { this->arithmeticExpression(functionCall); }
        catch (ParsingException &e) { this->conditionalExpression(functionCall);}
    }

    this->rBracket(functionCall);

    try { this->separator(functionCall); }
    catch (ParsingException &e){}
}

// block -> lBrace{1}
// (functionDef|functionCall|assignment|opIf|opReturn|opWhile|opDoWhile|opBreak|opContinue)?
// rBrace{1}

void Parser::block(ASTNode *parentPtr)
{
    ASTNode * block = new ASTNode({"block", "", curLineNum}, parentPtr);
    parentPtr->addChild(block);

    this->lBrace(block);

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
        catch (ParsingException & e) { this->rBrace(block); break; }
    }
}

// assignment -> variable{1}assignOp{1}(arithmeticExpression|conditionalExpression){1}

void Parser::assignment(ASTNode *parentPtr)
{
    ASTNode * assignment = new ASTNode({"assignment", "", curLineNum}, parentPtr);
    parentPtr->addChild(assignment);

    this->variable(assignment);

    this->assignOp(assignment);

    VecIt fixedIt = listIt;
    try { this->arithmeticExpression(assignment); this->endLine(assignment); }
    catch (ParsingException & e) { listIt = fixedIt; this->conditionalExpression(assignment); }
}

// allocation -> new{1}constructor{1}lBracket{1}(value{1}(argsSeparator{1}
// (arithmeticExpression{1}|conditionalExpression{1}))*)?rBracket{1}separator?

void Parser::allocation(ASTNode *parentPtr)
{
    ASTNode * allocation = new ASTNode({"allocation", "", curLineNum}, parentPtr);
    parentPtr->addChild(allocation);

    this->keyword("new", allocation);

    this->constructor(allocation);

    this->lBracket(allocation);

    try { this->value(allocation); }
    catch (ParsingException &e) 
    { 
        this->rBracket(allocation);
        return;
    }
    while ((*listIt)->type != "EOF")
    {
        try { this->argsSeparator(allocation); }
        catch (ParsingException &e) { break; }
        try { this->arithmeticExpression(allocation); }
        catch (ParsingException &e) { this->conditionalExpression(allocation);}
    }

    this->rBracket(allocation);

    try { this->separator(allocation); }
    catch (ParsingException &e){}

}

// arithmeticExpression -> ( (lBracket{1}arithmeticExpression{1}rBracket{1}mathOp{1})?notEndLine{1}
// unaryOP?value{1}unaryOp?((mathOp{1}arithmeticExpression{1})|(separator?)){1}

void Parser::arithmeticExpression(ASTNode *parentPtr)
{
    ASTNode * arithmeticExpression = new ASTNode({"arithmeticExpression", "", curLineNum}, parentPtr);
    parentPtr->addChild(arithmeticExpression);

    do 
    {
        try
        {
            try { this->lBracket(arithmeticExpression); }
            catch (ParsingException & e) { break; }
            this->arithmeticExpression(arithmeticExpression);
            this->rBracket(arithmeticExpression);
            try { this->mathOp(arithmeticExpression); }
            catch (ParsingException & e) { return; }
        }
        catch (ParsingException & e) { throw e; }
    } while (false);

    this->notEndLine(arithmeticExpression);
    try { this->unaryOp(arithmeticExpression); }
    catch(ParsingException & e){}
    try { this->value(arithmeticExpression); }
    catch (ParsingException &e) 
    { 
        try { this->functionCall(arithmeticExpression); }
        catch (ParsingException &e) { this->allocation(arithmeticExpression); } 
    }
    try { this->unaryOp(arithmeticExpression); }
    catch(ParsingException & e){}

    try { this->mathOp(arithmeticExpression); }
    catch (ParsingException & e)
    {
        try { this->separator(arithmeticExpression); }
        catch (ParsingException & e){}
        return;
    }
    this->arithmeticExpression(arithmeticExpression);
}

// conditionalExpression -> ( lBracket{1}(conditionalExpression{1}rBracket{1}(comprOp{1}|logicalOp))?
// notEndLine{1}arithmeticExpression{1}(((comprOp{1}|logicalOp{1})conditionalExpression{1})|
// (separator?)){1}

void Parser::conditionalExpression(ASTNode *parentPtr)
{
    ASTNode * conditionalExpression = new ASTNode({"conditionalExpression", "", curLineNum}, parentPtr);
    parentPtr->addChild(conditionalExpression);

    try { this->logicalNegation(conditionalExpression); }
    catch (ParsingException & e){}

    do 
    {
        try
        {
            try { this->lBracket(conditionalExpression); }
            catch (ParsingException & e) { break; }
            this->conditionalExpression(conditionalExpression);
            this->rBracket(conditionalExpression);
            try { this->comprOp(conditionalExpression); break; }
            catch (ParsingException & e) {}
            try { this->logicalOp(conditionalExpression); }
            catch (ParsingException & e) { return; }
        }
        catch (ParsingException & e) { throw e; }
    } while (false);

    this->notEndLine(conditionalExpression);
    this->arithmeticExpression(conditionalExpression);

    do
    {
        try { this->comprOp(conditionalExpression); break; }
        catch (ParsingException & e){}
        try { this->logicalOp(conditionalExpression); }
        catch (ParsingException & e)
        {
            try { this->separator(conditionalExpression); }
            catch (ParsingException & e){}
            return;
        }
    } while (false);

    this->conditionalExpression(conditionalExpression);
}

// opReturn -> (return){1}(arithmeticExpression{1}|conditionalExpression{1}|allocation{1})?separator?

void Parser::opReturn(ASTNode *parentPtr)
{
    ASTNode * opReturn = new ASTNode({"opReturn", "", curLineNum}, parentPtr);
    parentPtr->addChild(opReturn);

    this->keyword("return", opReturn);

    VecIt fixedIt = listIt;
    try { this->arithmeticExpression(opReturn); this->endLine(opReturn); }
    catch (ParsingException & e) { listIt = fixedIt; this->conditionalExpression(opReturn); }
}

// opBreak -> (break){1}separator?

void Parser::opBreak(ASTNode *parentPtr)
{
    ASTNode * opBreak = new ASTNode({"opBreak", "", curLineNum}, parentPtr);
    parentPtr->addChild(opBreak);

    this->keyword("break", opBreak);

    try { this->separator(opBreak); }
    catch (ParsingException &e){}
}

// opContinue -> (continue){1}separator?

void Parser::opContinue(ASTNode *parentPtr)
{
    ASTNode * opContinue = new ASTNode({"opContinue", "", curLineNum}, parentPtr);
    parentPtr->addChild(opContinue);

    this->keyword("continue", opContinue);

    try { this->separator(opContinue); }
    catch (ParsingException &e){}
}

// opIf -> (if){1}lBracket{1}conditionalExpression{1}rBracket{1}block{1}opElif?opElse?

void Parser::opIf(ASTNode *parentPtr)
{
    ASTNode * opIf = new ASTNode({"opIf", "", curLineNum}, parentPtr);
    parentPtr->addChild(opIf);

    this->keyword("if", opIf);
    this->lBracket(opIf);
    this->conditionalExpression(opIf);
    this->rBracket(opIf);
    this->block(opIf);

    try { this->opElif(opIf); }
    catch (ParsingException & e){}

    try { this->opElse(opIf); }
    catch (ParsingException & e){}
}

// opElif -> (elif){1}lBracket{1}conditionalExpression{1}rBracket{1}block{1}opElif?opElse?

void Parser::opElif(ASTNode *parentPtr)
{
    ASTNode * opElif = new ASTNode({"opElif", "", curLineNum}, parentPtr);
    parentPtr->addChild(opElif);

    this->keyword("elif",opElif);
    this->lBracket(opElif);
    this->conditionalExpression(opElif);
    this->rBracket(opElif);
    this->block(opElif);

    try { this->opElif(opElif); }
    catch (ParsingException & e){}

    try { this->opElse(opElif); }
    catch (ParsingException & e){}
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
    this->lBracket(opDoWhile);
    this->conditionalExpression(opDoWhile);
    this->rBracket(opDoWhile);
    try { this->separator(opDoWhile); }
    catch(ParsingException & e){}
}

// opWhile -> (while){1}lBracket{1}conditionalExpression{1}rBracket{1}block{1}

void Parser::opWhile(ASTNode *parentPtr)
{
    ASTNode * opWhile = new ASTNode({"opWhile", "", curLineNum}, parentPtr);
    parentPtr->addChild(opWhile);

    this->keyword("while", opWhile);
    this->lBracket(opWhile);
    this->conditionalExpression(opWhile);
    this->rBracket(opWhile);
    this->block(opWhile);
}

/*-------------------------------------------------ATHOMS---------------------------------------------*/

void Parser::keyword(string concrete, ASTNode *parentPtr)
{
    if (std::strcmp((*listIt)->value.c_str(), concrete.c_str()) != 0) 
    {
        throw ParsingException(generateException(concrete, (*listIt)->type)); 
    }
    else { listIt++; }
}

void Parser::constructor(ASTNode *parentPtr)
{
    if ((*listIt)->type != "CONSTRUCTOR")
    {
        throw ParsingException(generateException(string("constructor"), (*listIt)->type));
    }
    else { listIt++; }
}

void Parser::function(ASTNode *parentPtr)
{
    if ((*listIt)->type != "FUNCTION")
    {
        throw ParsingException(generateException(string("function"), (*listIt)->type));
    }
    else { listIt++; }
}

void Parser::lBracket(ASTNode *parentPtr)
{
    if ((*listIt)->type != "L_BRACKET")
    {
        throw ParsingException(generateException(std::string("'('"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::variable(ASTNode *parentPtr)
{
    if ((*listIt)->type != "VARIABLE")
    {
        throw ParsingException(generateException(std::string("variable"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::argsSeparator(ASTNode *parentPtr)
{
    if ((*listIt)->type != "ARG_SEPARATOR")
    {
        throw ParsingException(generateException(std::string("','"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::rBracket(ASTNode *parentPtr)
{
    if ((*listIt)->type != "R_BRACKET")
    {
        throw ParsingException(generateException(std::string("')'"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::lBrace(ASTNode *parentPtr)
{
    if ((*listIt)->type != "L_BRACE")
    {
        throw ParsingException(generateException(std::string("'{'"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::rBrace(ASTNode *parentPtr)
{
    if ((*listIt)->type != "R_BRACE")
    {
        throw ParsingException(generateException(std::string("'}'"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::assignOp(ASTNode *parentPtr)
{
    if ((*listIt)->type != "ASSIGN_OPERATOR")
    {
        throw ParsingException(generateException(std::string("'='"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::unaryOp(ASTNode *parentPtr)
{
    if ((*listIt)->type != "UNARY_OPERATOR")
    {
        throw ParsingException(generateException(std::string("unary operator"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::logicalNegation(ASTNode *parentPtr)
{
    if ((*listIt)->type != "LOGICAL_NEGATION")
    {
        throw ParsingException(generateException(std::string("'!'"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::value(ASTNode *parentPtr)
{
    if ((*listIt)->type != "INTEGER" and (*listIt)->type != "STRING" and
             (*listIt)->type != "VARIABLE" and (*listIt)->type != "FLOAT" and
             (*listIt)->type != "CONSTANT_KW")
    {
        throw ParsingException(generateException(std::string("value"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::mathOp(ASTNode *parentPtr)
{
    if ((*listIt)->type != "MATH_OPERATOR")
    {
        throw ParsingException(generateException(std::string("math. operator"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::separator(ASTNode *parentPtr)
{
    if ((*listIt)->type != "SEPARATOR")
    {
        throw ParsingException(generateException(std::string("';'"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::comprOp(ASTNode *parentPtr)
{
    if ((*listIt)->type != "COMPRASION_OPERATOR")
    {
        throw ParsingException(generateException(std::string("compr. operator"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::logicalOp(ASTNode *parentPtr)
{
    if ((*listIt)->value != "and" and (*listIt)->value != "or" and
        (*listIt)->value != "nand" and (*listIt)->value != "nor")
    {
        throw ParsingException(generateException(std::string("logical operator"), (*listIt)->type));
    }
    else { listIt++; }   
}

void Parser::endLine(ASTNode *parentPtr)
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

void Parser::notEndLine(ASTNode *parentPtr)
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
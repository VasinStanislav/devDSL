#include "../headers/AST.h"

/*----------------------------------------------ASTNODE-----------------------------------------------*/

ASTNode::ASTNode() : label(), parentPtr(nullptr) {}

ASTNode::ASTNode(ASTNode *parentPtr) : label(), parentPtr(parentPtr) {}

ASTNode::ASTNode(Token label, ASTNode *parentPtr) : label(label), parentPtr(parentPtr) {}

ASTNode::~ASTNode()
{
    for (auto childIt = this->children.begin(); childIt != this->children.end(); childIt++)
    {
        delete *childIt;
    }
    this->children.clear();
}

void ASTNode::setParentPtr(ASTNode *parentPtr) { this->parentPtr = parentPtr; }

ASTNode * ASTNode::getParentPtr() { return parentPtr; }

void ASTNode::setLabel(Token label) { this->label = label; }

void ASTNode::setLabel(ConstStrRef value, ConstStrRef type, unsigned int line) 
{ 
    label = {value, type, line};
}

Token ASTNode::getLabel() { return label; }

void ASTNode::addChild(ASTNode *child) { this->children.push_back(child); }

ASTNode * ASTNode::getLastChild()
{
    if (this->children.empty()) { return nullptr; }
    return this->children.back();
}

void ASTNode::deleteFirstChild()
{
    if (this->children.empty()) { return; }
    auto first = this->children.begin();
    this->children.erase(first);
}

void ASTNode::deleteLastChild() 
{
    if (this->children.empty()) { return; }
    this->children.pop_back();
}

size_t ASTNode::getChildrenAmount() { return children.size(); }

NodeV * ASTNode::getChildrenPtr() { return &children; }

ASTNode * ASTNode::getLastNode()
{
    if (this->children.empty()) { return this; }
    else { return this->children.back()->getLastNode(); }
}

void ASTNode::showTree()
{
    if (children.empty()) { return; }
    std::cout<<this->label.value<<"\n";
    for (const auto child : this->children) { std::cout<<"  "<<child->getLabel().value; }
    std::cout<<"\n";
    for (const auto child : this->children) { child->showTree(); }
}

void swap(ASTNode *opPrev, ASTNode *opNext)
{
    // '='
    ASTNode * ancestorPtr = opPrev->parentPtr;
    ancestorPtr->deleteLastChild();
    // '+'
    opNext->parentPtr     = ancestorPtr;
    ancestorPtr->addChild(opNext);
    // '-'
    opPrev->parentPtr     = opNext;
    opNext->addChild(opPrev);
}

bool isEqualWith(ASTNode & node, ConstStrRef strRef)
{
    // node.label.
    return false;
}

/*--------------------------------------------------AST-----------------------------------------------*/

AST::AST(Token label) : ASTNode(label) {}

ASTNode * AST::getLastNode()
{
    if (this->getChildrenPtr()->empty()) { return nullptr; }
    else { return this->getChildrenPtr()->back()->getLastNode(); }
}

void AST::deleteLastNode()
{
    auto lastNode = this->getLastNode()->getParentPtr();
    if (lastNode) { this->getChildrenPtr()->pop_back(); }
}

/*-----------------------------------------PRECEDENCY CONTROL-----------------------------------------*/

const PriorityMap PredencyControl::PRIORITY_MAP = {{"*", 0}, {"\\", 0}, {"%", 0}, {"+", 1}, {"-", 1}, 
    {">", 2}, {"<", 2}, {">=", 2}, {"<=", 2}, {"==", 3}, {"!=", 3}, {"and", 4}, {"or", 4}, {"=", 5}, 
    {"callee", 5}, {"new", 5}, {"return", 5}, {"value", 5}, {"if", 5}, {"elif", 5}, {"while",5}};

int PredencyControl::curPriority;

void PredencyControl::initPriority() { curPriority = 0; }

void PredencyControl::setCurPriority(string op) { curPriority = PRIORITY_MAP.at(op); }

void PredencyControl::setCurPriority(int predency) { curPriority = predency; }

int PredencyControl::getCurPriority() { return curPriority; }

int PredencyControl::getPredency(string op) { return PRIORITY_MAP.at(op); }
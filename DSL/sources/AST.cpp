#include "../headers/AST.h"

/*-------------------------------------------ASTNODE--------------------------------------------*/

ASTNode::ASTNode() : label(), parentPtr(nullptr) {}

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

void ASTNode::deleteLastChild() 
{
    if (this->children.empty()) { return; }
    this->children.pop_back();
}

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

/*------------------------------------------------AST-------------------------------------------*/

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
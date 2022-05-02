#include "../headers/AST.h"

/*-------------------------------------------ASTNODE--------------------------------------------*/

ASTNode::ASTNode() : label(), parentPtr(nullptr) {}

ASTNode::ASTNode(Token label, ASTNode *parentPtr) : label(label), parentPtr(parentPtr) {}

ASTNode::ASTNode(ConstStrRef value, ConstStrRef type, unsigned int line, ASTNode *parentPtr)
    : label({value, type, line}), parentPtr(parentPtr) {}

ASTNode::~ASTNode()
{
    for (auto childIt = this->children.begin(); childIt != this->children.end(); childIt++)
    {
        delete *childIt;
    }
    this->children.clear();
}

void ASTNode::setLabel(ConstStrRef value, ConstStrRef type, unsigned int line) 
{ 
    label = {value, type, line};
}

Token ASTNode::getLabel() { return label; }

void ASTNode::addChild(ASTNode *child) { this->children.push_back(child); }

NodeV ASTNode::getChildren() { return children; }

void ASTNode::showTree()
{
    if (children.empty()) { return; }
    std::cout<<this->label.value<<"\n";
    for (const auto child : this->children) { std::cout<<"  "<<child->getLabel().value; }
    std::cout<<"\n";
    for (const auto child : this->children) { child->showTree(); }
}

/*------------------------------------------------AST-------------------------------------------*/
/*
AST::AST(Token label)      : root(label) {}

AST::AST(ASTNode root) : root(root) {}*/
#ifndef EXPRASTNode_H
#define EXPRASTNode_H

#include <iostream>
#include <string>
#include <vector>
#include "Token.h"

/*-base-*/ class ASTNode; 

typedef std::string                 string;
typedef const std::string &         ConstStrRef;
typedef std::pair<string, string>   StrPair;
typedef const StrPair               ConstStrPairRef;
typedef std::vector<string>         StrV;
typedef const StrV &                ConstStrVRef;
typedef std::vector<ASTNode *>      NodeV;

/*----------------------------------------------BASE--------------------------------------------------*/

class ASTNode 
{
        ASTNode *parentPtr;
        Token    label;
        NodeV    children;
    public:
        ASTNode();
        ASTNode(Token, ASTNode *parentPtr=nullptr);
        ASTNode(ConstStrRef, ConstStrRef, unsigned int, ASTNode *parentPtr=nullptr);
     /*-virtual-*/ ~ASTNode();

        void setLabel(Token);
        void setLabel(ConstStrRef, ConstStrRef, unsigned int);
        Token getLabel();

        void setParentPtr(ASTNode *);
        ASTNode * getParentPtr();

        void addChild(ASTNode *);
        NodeV getChildren();

        void showTree();
};

/*class AST
{
        ASTNode root;
    public:
        AST(Token label);
        AST(ASTNode root);
};*/

/*-----------------------------------------LITERALS, OPERATIONS---------------------------------------*/
/*
class IntASTNode : public ASTNode 
{
        long long value;
    public:
        IntASTNode(long long value) : ASTNode("INTEGER"), value(value) {}
};

class FloatASTNode : public ASTNode 
{
        double long value;
    public:
        FloatASTNode(double long value) : ASTNode("FLOAT"), value(value) {}
};

class StringASTNode : public ASTNode 
{
        string value;
    public:
        StringASTNode(ConstStrRef &value) : ASTNode("STRING"), value(value) {}
};

class VariableASTNode : public ASTNode 
{
        string name;
    public:
        VariableASTNode(ConstStrRef name, ConstStrRef type)
            : ASTNode(type), name(name) {}
};

class BinaryOpASTNode : public ASTNode
{
        char op;
        ASTNode *lHS, *rHS;
    public:
        BinaryOpASTNode(ASTNode *lHS, ASTNode *rHS, char op)
            : lHS(lHS), rHS(rHS), op(op) {}
};
*/
/*------------------------------------FUNCTION DEFS, FUNCTION CALLS-----------------------------------*/
/*
class FuncCallASTNode : public ASTNode
{
        string call;
        NodeV args;
    public:
        FuncCallASTNode(ConstStrRef &call, NodeV &args)
            : call(call), args(args) {}
};

class FuncPrototypeASTNode : public ASTNode 
{
        string name;
        StrV args;
    public:
        FuncPrototypeASTNode(ConstStrRef name, ConstStrVRef args)
            : name(name), args(args) {}
};

class FunctionASTNode {
        FuncPrototypeASTNode *proto;
        ASTNode *block;
    public:
        FunctionASTNode(FuncPrototypeASTNode *proto, ASTNode *block)
            : proto(proto), block(block) {}
};
*/
#endif
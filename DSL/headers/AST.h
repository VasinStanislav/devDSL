#ifndef EXPRASTNode_H
#define EXPRASTNode_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Token.h"

/*-base-*/ class ASTNode; 

typedef std::string                  string;
typedef const std::string &          ConstStrRef;
typedef std::pair<string, string>    StrPair;
typedef const StrPair                ConstStrPairRef;
typedef std::vector<string>          StrV;
typedef const StrV &                 ConstStrVRef;
typedef std::vector<ASTNode *>       NodeV;
typedef std::map<string, int>        PriorityMap;

/*----------------------------------------------BASE--------------------------------------------------*/

class ASTNode 
{
        friend bool isEqualWith(ASTNode &, ConstStrRef);
        friend void swap(ASTNode *, ASTNode *);

        ASTNode *parentPtr;
        Token    label;
        NodeV    children;
    protected:
        NodeV *getChildrenPtr();
    public:
        ASTNode();
        ASTNode(ASTNode *);
        ASTNode(Token, ASTNode *parentPtr=nullptr);
     /*-virtual-*/ ~ASTNode();

        void setParentPtr(ASTNode *);
        ASTNode * getParentPtr();

        void setLabel(Token);
        void setLabel(ConstStrRef, ConstStrRef, unsigned int);
        Token getLabel();

        void addChild(ASTNode *);

                ASTNode * getFirstChild();
                ASTNode * getLastChild();
        virtual ASTNode * getLastNode();
        void deleteFirstChild();
        void deleteLastChild();

        void showTree();
};

/*----------------------------------------ABSTRACT SYNTAX TREE----------------------------------------*/

class AST : public ASTNode
{
    public:
        AST(Token);
        ASTNode * getLastNode();
        void deleteLastNode();
};

/*------------------------------------------PREDENCY CONTROL------------------------------------------*/

class PredencyControl
{
        const static PriorityMap PRIORITY_MAP;
        static int curPriority;
    public:
        static void initPriority();
        static void setCurPriority(string);
        static void setCurPriority(int);
        static int  getCurPriority();
        static int  getPredency(string);
};

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
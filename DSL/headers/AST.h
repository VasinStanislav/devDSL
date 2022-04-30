#ifndef EXPRASTNode_H
#define EXPRASTNode_H

#include <string>
#include <vector>

/*-base-*/ class ExprASTNode; 

typedef std::string                 string;
typedef const std::string &         ConstStrRef;
typedef std::pair<string, string>   StrPair;
typedef const StrPair               ConstStrPairRef;
typedef std::vector<string>         StrV;
typedef const StrV &                ConstStrVRef;
typedef std::vector<ExprASTNode *>  NodeV;

/*----------------------------------------------BASE--------------------------------------------------*/

class ExprASTNode 
{
        StrPair label;
        NodeV children;
    public:
        ExprASTNode(ConstStrRef name="", ConstStrRef type="") : label(std::make_pair(name, type)) {}
        ExprASTNode(StrPair label) : label(label) {}
        virtual ~ExprASTNode() { for (size_t i = 0; i < children.size(); ++i) { delete children[i]; } }

        void setLabel(ConstStrRef name, ConstStrRef type) { this->label = std::make_pair(name, type); }
        void setLabel(StrPair label) { this->label = label; }
        ConstStrPairRef getLabel() { return this->label; }
        void addChild(ExprASTNode *child) { this->children.push_back(child); }
        NodeV getChildren() { return children; }                                                  
};

class AST
{
        ExprASTNode root;
    public:
        AST(StrPair label)    : root(ExprASTNode(label)){}
        AST(ExprASTNode root) : root(ExprASTNode(root)){}
};

/*-----------------------------------------LITERALS, OPERATIONS---------------------------------------*/
/*
class IntExprASTNode : public ExprASTNode 
{
        long long value;
    public:
        IntExprASTNode(long long value) : ExprASTNode("INTEGER"), value(value) {}
};

class FloatExprASTNode : public ExprASTNode 
{
        double long value;
    public:
        FloatExprASTNode(double long value) : ExprASTNode("FLOAT"), value(value) {}
};

class StringExprASTNode : public ExprASTNode 
{
        string value;
    public:
        StringExprASTNode(ConstStrRef &value) : ExprASTNode("STRING"), value(value) {}
};

class VariableExprASTNode : public ExprASTNode 
{
        string name;
    public:
        VariableExprASTNode(ConstStrRef name, ConstStrRef type)
            : ExprASTNode(type), name(name) {}
};

class BinaryOpExprASTNode : public ExprASTNode
{
        char op;
        ExprASTNode *lHS, *rHS;
    public:
        BinaryOpExprASTNode(ExprASTNode *lHS, ExprASTNode *rHS, char op)
            : lHS(lHS), rHS(rHS), op(op) {}
};
*/
/*------------------------------------FUNCTION DEFS, FUNCTION CALLS-----------------------------------*/
/*
class FuncCallExprASTNode : public ExprASTNode
{
        string call;
        NodeV args;
    public:
        FuncCallExprASTNode(ConstStrRef &call, NodeV &args)
            : call(call), args(args) {}
};

class FuncPrototypeASTNode : public ExprASTNode 
{
        string name;
        StrV args;
    public:
        FuncPrototypeASTNode(ConstStrRef name, ConstStrVRef args)
            : name(name), args(args) {}
};

class FunctionASTNode {
        FuncPrototypeASTNode *proto;
        ExprASTNode *block;
    public:
        FunctionASTNode(FuncPrototypeASTNode *proto, ExprASTNode *block)
            : proto(proto), block(block) {}
};
*/
#endif
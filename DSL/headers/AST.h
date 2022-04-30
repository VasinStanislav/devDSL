#ifndef EXPRAST_H
#define EXPRAST_H

#include <string>
#include <vector>

/*-------------------------------------------BASE-----------------------------------------------*/

class ExprAST 
{
        std::string type;
    public:
        ExprAST(const std::string &type="") : type(type) {}
        virtual ~ExprAST() {}

        void setType(const std::string &type) { this->type = type; }
        const std::string &getType() { return this->type; }
};

/*-----------------------------------LITERALS, OPERATIONS---------------------------------------*/

class IntExprAST : public ExprAST 
{
        long long value;
    public:
        IntExprAST(long long value) : ExprAST("INTEGER"), value(value) {}
};

class FloatExprAST : public ExprAST 
{
        double long value;
    public:
        FloatExprAST(double long value) : ExprAST("FLOAT"), value(value) {}
};

class StringExprAST : public ExprAST 
{
        std::string value;
    public:
        StringExprAST(const std::string &value) : ExprAST("STRING"), value(value) {}
};

class VariableExprAST : public ExprAST 
{
        std::string name;
    public:
        VariableExprAST(const std::string &name, const std::string &type)
            : ExprAST(type), name(name) {}
};

class BinaryOpExprAST : public ExprAST
{
        char op;
        ExprAST *lHS, *rHS;
    public:
        BinaryOpExprAST(ExprAST *lHS, ExprAST *rHS, char op)
            : lHS(lHS), rHS(rHS), op(op) {}
};

/*----------------------------------FUNCTION DEFS, FUNCTION CALLS-------------------------------*/

class FuncCallExprAST : public ExprAST
{
        std::string call;
        std::vector<ExprAST *> args;
    public:
        FuncCallExprAST(const std::string &call, std::vector<ExprAST *> &args)
            : call(call), args(args) {}
};

class FuncPrototypeAST : public ExprAST 
{
        std::string name;
        std::vector<std::string> args;
    public:
        FuncPrototypeAST(const std::string &name, const std::vector<std::string> &args)
            : name(name), args(args) {}
};

class FunctionAST {
        FuncPrototypeAST *proto;
        ExprAST *block;
    public:
        FunctionAST(FuncPrototypeAST *proto, ExprAST *block)
            : proto(proto), block(block) {}
};

#endif
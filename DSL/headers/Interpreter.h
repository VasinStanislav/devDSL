#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <map>
#include <cstring>
#include "StackMachine.h"

typedef std::map<std::string, long>        IntVariables;
typedef std::map<std::string, double>      FloatVariables;
typedef std::map<std::string, bool>        BooleanVariables;
typedef std::map<std::string, std::string> StrVariables;

class Interpreter
{
        Content        * staticMem;
        NodeVector     * functions;

        IntVariables     staticIntVars;
        IntVariables     stackIntVars;
        FloatVariables   staticFloatVars;
        FloatVariables   stackFloatVars;
        BooleanVariables staticBooleanVars;
        BooleanVariables stackBooleanVars;
        StrVariables     staticStrVars;
        StrVariables     stackStrVars;

        Stack stack;
    public:
        Interpreter(Content *, NodeVector *);
        Interpreter(Memory);
        Interpreter(Content *);
        ~Interpreter();

        void run();

        void addStaticVariable(string, Token);
        Token doBinaryOp(Token, Token, Token);
};

#endif
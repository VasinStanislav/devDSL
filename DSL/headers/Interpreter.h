#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <map>
#include <set>
#include <cstring>
#include "StackMachine.h"

typedef std::map<std::string, long>        IntVariables;
typedef std::map<std::string, double>      FloatVariables;
typedef std::map<std::string, bool>        BooleanVariables;
typedef std::map<std::string, std::string> StrVariables;

typedef std::vector<string>                StringVector;
typedef std::set<string>                   StringSet;

enum Scope { local, function, global };

class Interpreter
{
        Content        * staticMem;
        NodeVector     * functions;

        IntVariables     staticIntVars;
        IntVariables     funcIntVars;
        IntVariables     stackIntVars;
        FloatVariables   staticFloatVars;
        FloatVariables   funcFloatVars;
        FloatVariables   stackFloatVars;
        BooleanVariables staticBooleanVars;
        BooleanVariables funcBooleanVars;
        BooleanVariables stackBooleanVars;
        StrVariables     staticStrVars;
        StrVariables     funcStrVars;
        StrVariables     stackStrVars;

        Stack stack;
        Scope mode;
    public:
        Interpreter(Content *, NodeVector *);
        Interpreter(Memory);
        Interpreter(Content *);
        ~Interpreter();

        void run();

        void showVariables();
    private:
        Token runFunction(Content *);

        void addStaticVariable(string, Token);
        Token doBinaryOp(Token, Token, Token);
        Token doUnaryOp(Token, Token);

        void specifyVariable(Token *);
        void normalizeBoolean(Token *);
        void normalizeString(Token *);
        template<typename T>
        T normalizeValue(Token);
};

#endif
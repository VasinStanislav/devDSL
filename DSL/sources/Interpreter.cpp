#include "../headers/Interpreter.h"

Interpreter::Interpreter(Content *staticMem, NodeVector *functions) 
    : staticMem(staticMem), functions(functions){}

Interpreter::Interpreter(Memory mem) : staticMem(mem.first), functions(mem.second){}

Interpreter::Interpreter(Content *staticMem) : staticMem(staticMem), functions(nullptr){}

Interpreter::~Interpreter()
{
    for (auto &line : *staticMem)
    {
        delete line;
    }
    delete staticMem;
}

/*----------------------------------------------RUNTIME-----------------------------------------------*/

// testing
void Interpreter::run()
{
    Stack curStack;
    for (const auto &line : *(this->staticMem))
    {
        while (!line->empty())
        {
            auto el = line->top();
            line->pop();

            if (el.type == "INTEGER" or el.type == "STRING" or el.type == "VARIABLE" or
                el.type == "FLOAT" or el.type == "CONSTANT_KW")
            {
                curStack.push(el);
            }
            else if (el.type == "MATH_OPERATOR" or el.type == "COMPRASION_OPERATOR" or
                     el.type == "LOGICAL_KW")
            {
                auto el1 = curStack.top();
                curStack.pop();
                auto el2 = curStack.top();
                curStack.pop();
                auto op = el;

                auto res = this->doBinaryOp(el2, el1, op);
                curStack.push(res);
            }
            else if (el.type == "ASSIGN_OPERATOR")
            {
                auto newVar = curStack.top();
                curStack.pop();
                auto value = curStack.top();
                curStack.pop();

                this->addStaticVariable(newVar.value, value);
            }

        }
    }

    for (const auto &el : this->staticStrVars)
    {
        std::cout<<el.first<<" "<<el.second<<" <- string"<<"\n";
    }
    for (const auto &el : this->staticBooleanVars)
    {
        std::cout<<el.first<<" "<<el.second<<" <- boolean"<<"\n";
    }
    for (const auto &el : this->staticFloatVars)
    {
        std::cout<<el.first<<" "<<el.second<<" <- float"<<"\n";
    }
    for (const auto &el : this->staticIntVars)
    {
        std::cout<<el.first<<" "<<el.second<<" <- int"<<"\n";
    }
}

Token Interpreter::doBinaryOp(Token el1, Token el2, Token op)
{
    string type = el1.type;
    if (std::strcmp(type.c_str(), el2.type.c_str())!=0)
    {
        std::cout<<"rip\n";
        return {"", "undefined", el1.line};
    }

    string res;
    if (type == "INTEGER")
    {
        if      (op.value == "+")  { res = std::to_string(std::stol(el1.value) + std::stol(el2.value)); }
        else if (op.value == "-")  { res = std::to_string(std::stol(el1.value) - std::stol(el2.value)); }
        else if (op.value == "*")  { res = std::to_string(std::stol(el1.value) * std::stol(el2.value)); }
        else if (op.value == "%")  { res = std::to_string(std::stol(el1.value) % std::stol(el2.value)); }
        else if (op.value == "\\") { res = std::to_string(std::stol(el1.value) / std::stol(el2.value)); }
        else if (op.value == ">")  { res = std::to_string(std::stol(el1.value) > std::stol(el2.value)); }
        else if (op.value == "<")  { res = std::to_string(std::stol(el1.value) < std::stol(el2.value)); }
        else if (op.value == ">=") { res = std::to_string(std::stol(el1.value) >= std::stol(el2.value)); }
        else if (op.value == "<=") { res = std::to_string(std::stol(el1.value) <= std::stol(el2.value)); }
        else if (op.value == "==") { res = std::to_string(std::stol(el1.value) == std::stol(el2.value)); }
        else if (op.value == "!=") { res = std::to_string(std::stol(el1.value) != std::stol(el2.value)); }
    }
    else if (type == "STRING")
    {
        if (op.value == "+")  { res = el1.value.substr(1, el1.value.size()-2) + el2.value.substr(1, el1.value.size()-2); }
    }
    else if (type == "FLOAT")
    {
        if      (op.value == "+")  { res = std::to_string(std::stod(el1.value) + std::stod(el2.value)); }
        else if (op.value == "-")  { res = std::to_string(std::stod(el1.value) - std::stod(el2.value)); }
        else if (op.value == "*")  { res = std::to_string(std::stod(el1.value) * std::stod(el2.value)); }
        else if (op.value == "\\") { res = std::to_string(std::stod(el1.value) / std::stod(el2.value)); }
        else if (op.value == ">")  { res = std::to_string(std::stod(el1.value) > std::stod(el2.value)); }
        else if (op.value == "<")  { res = std::to_string(std::stod(el1.value) < std::stod(el2.value)); }
        else if (op.value == ">=") { res = std::to_string(std::stod(el1.value) >= std::stod(el2.value)); }
        else if (op.value == "<=") { res = std::to_string(std::stod(el1.value) <= std::stod(el2.value)); }
        else if (op.value == "==") { res = std::to_string(std::stod(el1.value) == std::stod(el2.value)); }
        else if (op.value == "!=") { res = std::to_string(std::stod(el1.value) != std::stod(el2.value)); }
    }
    else if (type == "CONSTANT_KW")
    {
        this->normalizeBoolean(&el1);
        this->normalizeBoolean(&el2);
        if      (op.value == ">")   { res = std::to_string(std::stoi(el1.value) > std::stoi(el2.value)); }
        else if (op.value == "<")   { res = std::to_string(std::stoi(el1.value) < std::stoi(el2.value)); }
        else if (op.value == ">=")  { res = std::to_string(std::stoi(el1.value) >= std::stoi(el2.value)); }
        else if (op.value == "<=")  { res = std::to_string(std::stoi(el1.value) <= std::stoi(el2.value)); }
        else if (op.value == "==")  { res = std::to_string(std::stoi(el1.value) == std::stoi(el2.value)); }
        else if (op.value == "!=")  { res = std::to_string(std::stoi(el1.value) != std::stoi(el2.value)); }
        else if (op.value == "and") { res = std::to_string(std::stoi(el1.value) and std::stoi(el2.value)); }
        else if (op.value == "or")  { res = std::to_string(std::stoi(el1.value) or std::stoi(el2.value)); }
    }
    if (op.type == "COMPRASION_OPERATOR" or op.type == "LOGICAL_KW") { type = "CONSTANT_KW"; }

    return {res, type, el2.line};
}

void Interpreter::addStaticVariable(string varName, Token value)
{
    if (value.type == "INTEGER") { this->staticIntVars.insert({varName, std::stol(value.value)}); }
    else if (value.type == "STRING") { this->staticStrVars.insert({varName, value.value}); }
    else if (value.type == "FLOAT") { this->staticFloatVars.insert({varName, std::stol(value.value)}); }
    else if (value.type == "CONSTANT_KW") { this->staticBooleanVars.insert({varName, (bool)std::stoi(value.value)}); }
}

void Interpreter::normalizeBoolean(Token *booleanEl)
{
    if (booleanEl->value == "true") { booleanEl->value = "1"; }
    else { booleanEl->value = "0"; }
}
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
        Vector buffer;
        int buffIt = -1;
        StringVector currentLineVariables;
        while (!line->empty())
        {
            auto el = line->top();
            line->pop();

            buffer.push_back(el);
            buffIt++;

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
            else if (el.type == "UNARY_OPERATOR" or el.type == "LOGICAL_NEGATION")
            {
                auto el_ = curStack.top();
                curStack.pop();
                auto unOp = el;

                auto res = this->doUnaryOp(el_, unOp);
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
            else if (el.type == "pFalse")
            {
                auto condition = curStack.top();
                curStack.pop();

                this->normalizeBoolean(&condition);

                int pos = std::stoi(el.value.substr(0, el.value.find('!')));
                
                if ((bool)std::stoi(condition.value)) { continue; }
                
                while (buffIt != pos)
                {
                    auto el = line->top();
                    line->pop();

                    buffer.push_back(el);
                    buffIt++;
                }
            }
            else if (el.type == "p")
            {
                int pos = std::stoi(el.value.substr(0, el.value.find('!')));

                if (buffIt <= pos)
                {
                    while (buffIt != pos)
                    {
                        auto el = line->top();
                        line->pop();

                        buffer.push_back(el);
                        buffIt++;
                    }
                }
                else 
                {
                    //auto it 
                }
            }

        }
    }


}

Token Interpreter::doBinaryOp(Token el1, Token el2, Token op)
{
    if (el1.type == "VARIABLE") { this->specifyVariable(&el1); }
    if (el2.type == "VARIABLE") { this->specifyVariable(&el2); }

    string type = el1.type;

    if (std::strcmp(type.c_str(), el2.type.c_str())!=0 or el1.type.empty() or el2.type.empty())
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
        this->normalizeString(&el1);
        this->normalizeString(&el2);
        if (op.value == "+")  { res = el1.value + el2.value; }
        if (op.value == "==") { res = std::to_string(el1.value == el2.value); }
        if (op.value == "!=") { res = std::to_string(el1.value != el2.value); }
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

Token Interpreter::doUnaryOp(Token el, Token op)
{
    if (el.type == "VARIABLE") { this->specifyVariable(&el); }
    string type = el.type;

    string res;
    if (type == "INTEGER")
    {
        if (op.value == "~") { res = std::to_string(-(std::stol(el.value))); }
        else if (op.value == "++") { res = std::to_string((std::stol(el.value)) + 1l); }
        else if (op.value == "++") { res = std::to_string(-(std::stol(el.value)) - 1l); }
    }
    else if (type == "STRING")
    {
        this->normalizeBoolean(&el);
        string value = el.value;
        if (op.value == "++") { value.append(" "); res = value; }
        else if (op.value == "--") { value.pop_back(); res = value; }
    }
    else if (type == "FLOAT")
    {
        if (op.value == "~") { res = std::to_string(-(std::stod(el.value))); }
        else if (op.value == "++") { res = std::to_string(std::stod(el.value) + 1.0); }
        else if (op.value == "--") { res = std::to_string(std::stod(el.value) - 1.0); }
    }
    else if (type == "CONSTANT_KW")
    {
        this->normalizeBoolean(&el);
        string value = el.value;
        if (op.value == "!") 
        { 
            bool val = (bool)std::stoi(el.value);
            val = !val; 
            res = std::to_string(val); 
        }
    }

    return { res, type, el.line };
}

void Interpreter::addStaticVariable(string varName, Token value)
{
    bool isInserted;
    if (value.type == "INTEGER") 
    { 
        const auto [it, isInserted] = this->staticIntVars.insert({varName, std::stol(value.value)}); 
        if (!isInserted) 
        { 
            this->staticIntVars.erase(varName); 
            this->staticIntVars.insert({varName, std::stol(value.value)}); 
        }
    }
    else if (value.type == "STRING") 
    { 
        const auto [it, isInserted] = this->staticStrVars.insert({varName, value.value}); 
        if (!isInserted) { this->staticStrVars.find(value.value)->second = value.value; }
    }
    else if (value.type == "FLOAT") 
    { 
        const auto [it, isInserted] = this->staticFloatVars.insert({varName, std::stod(value.value)}); 
        if (!isInserted) { this->staticFloatVars.find(value.value)->second = std::stod(value.value); }
    }
    else if (value.type == "CONSTANT_KW") 
    { 
        const auto [it, isInserted] = this->staticBooleanVars.insert({varName, (bool)std::stoi(value.value)});
        if (!isInserted) { this->staticBooleanVars.find(value.value)->second = (bool)std::stoi(value.value); } 
    }
}

void Interpreter::normalizeBoolean(Token *booleanEl)
{
    if (std::strcmp(booleanEl->value.c_str(), "false") == 0) { booleanEl->value = "0"; }
    else if (std::strcmp(booleanEl->value.c_str(), "true") == 0) { booleanEl->value = "1"; }
}

void Interpreter::normalizeString(Token *stringEl)
{
    if (stringEl->value.at(0) == '"') 
    { 
        stringEl->value = stringEl->value.substr(1, stringEl->value.size()-2);
    }
}

void Interpreter::specifyVariable(Token *variable)
{
    string type;
    string value;

    
    if (this->stackBooleanVars.contains(variable->value.data())) 
    { 
        type = "BOOLEAN"; 
        auto buff = this->stackBooleanVars.find(variable->value.c_str()); 
        value = std::to_string(buff->second);
    }
    else if (this->stackFloatVars.contains(variable->value.data())) 
    { 
        type = "FLOAT"; 
        auto buff = this->stackFloatVars.find(variable->value.c_str());
        value = std::to_string(buff->second);
    }
    else if (this->stackIntVars.contains(variable->value.data())) 
    {
        type = "INTEGER"; 
        auto buff = this->stackIntVars.find(variable->value.c_str());
        value = std::to_string(buff->second);
    }
    else if (this->stackStrVars.contains(variable->value.data())) 
    { 
        type = "STRING"; 
        auto buff = this->stackStrVars.find(variable->value.c_str());
        value = buff->second;
    }
    else if (this->staticBooleanVars.contains(variable->value.data())) 
    { 
        type = "BOOLEAN"; 
        auto buff = this->staticBooleanVars.find(variable->value.c_str());
        value = std::to_string(buff->second);
    }
    else if (this->staticFloatVars.contains(variable->value.data())) 
    { 
        type = "FLOAT"; 
        auto buff = this->staticFloatVars.find(variable->value.c_str());
        value = std::to_string(buff->second);
    }
    else if (this->staticIntVars.contains(variable->value.data())) 
    { 
        type = "INTEGER"; 
        auto buff = this->staticIntVars.find(variable->value.c_str());
        value = std::to_string(buff->second);
    }
    else if (this->staticStrVars.contains(variable->value.data())) 
    { 
        type = "STRING";
        auto buff = this->staticStrVars.find(variable->value.c_str()); 
        value = buff->second;
    }

    variable->value = value;
    variable->type = type;
}

void Interpreter::showVariables()
{
    for (const auto &el : this->staticStrVars)
    {
        std::cout<<el.first<<" "<<el.second<<" <- static string"<<"\n";
    }
    for (const auto &el : this->staticBooleanVars)
    {
        std::cout<<el.first<<" "<<el.second<<" <- static boolean"<<"\n";
    }
    for (const auto &el : this->staticFloatVars)
    {
        std::cout<<el.first<<" "<<el.second<<" <- static float"<<"\n";
    }
    for (const auto &el : this->staticIntVars)
    {
        std::cout<<el.first<<" "<<el.second<<" <- static int"<<"\n";
    }
    for (const auto &el : this->stackStrVars)
    {
        std::cout<<el.first<<" "<<el.second<<" <- string"<<"\n";
    }
    for (const auto &el : this->stackBooleanVars)
    {
        std::cout<<el.first<<" "<<el.second<<" <- boolean"<<"\n";
    }
    for (const auto &el : this->stackFloatVars)
    {
        std::cout<<el.first<<" "<<el.second<<" <- float"<<"\n";
    }
    for (const auto &el : this->stackIntVars)
    {
        std::cout<<el.first<<" "<<el.second<<" <- int"<<"\n";
    }
}
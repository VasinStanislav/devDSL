#include "../headers/execApp.h"

int execApp()
{
    std::cout<<std::setw(29)<<std::setfill('*')<<std::right<<"DEV";
    std::cout<<std::setw(28)<<std::left<<"DSL"<<"\n";

    std::ifstream in("../resources/program.dsl");

    V list      = *analyze(&in);

    AST * tree = list.empty() ? nullptr : parse(&list);

    Memory mem = getRPN(tree);

    runProgram(mem);

    delete tree;

    std::cout<<std::setw(29)<<std::setfill('*')<<std::right<<"";
    std::cout<<std::setw(28)<<std::left<<""<<std::setfill(' ')<<"\n";

    return 0;
}

V * analyze(std::ifstream *input)
{
    Lexer lexer;

    V *tokenList = new V(0);
    if (input->is_open())
    {
        tokenList = lexer.tokenize(input);
    }
    else 
    {
        std::cerr<<"Oops!"<<"\n";
    }
    input->close();

    std::cout<<std::setw(29)<<std::right<<"Lex";
    std::cout<<std::setw(28)<<std::left<<"er"<<std::setfill(' ')<<"\n";

    for (const auto &el : *tokenList)
    {
        std::cout<<std::setw(32)<<std::left<<el->value.c_str()<<":";
        std::cout<<std::setw(24)<<std::right<<el->type.c_str()<<"\n";
    }

    return tokenList;
}

AST * parse(V *tokenList)
{
    std::cout<<std::setw(29)<<std::setfill('*')<<std::right<<"Par";
    std::cout<<std::setw(28)<<std::left<<"ser"<<"\n";

    Parser parser(tokenList);
    int res = parser.lang();

    AST * root = nullptr;
    if (res==0) 
    { 
        root = parser.getTree(); 
    }
    else       
    { 
        return root; 
    }

    std::cout<<std::setw(29)<<std::setfill('*')<<std::right<<"AS";
    std::cout<<std::setw(28)<<std::left<<"T"<<"\n";
    
    root->showTree();

    return root;
}

Memory getRPN(AST *root)
{
    StackMachine stackMachine;
    stackMachine.split(root);
    Content * heap = stackMachine.getHeap();
    NodeVector * functions = stackMachine.getFunctions();

    std::cout<<std::setw(29)<<std::setfill('*')<<std::right<<"STACK";
    std::cout<<std::setw(28)<<std::left<<"MACHINE"<<"\n";

    std::cout<<"HEAP CONTAINS:\n";

    for (const auto &line : *heap)
    {
        struct hack : Stack
        {
            static Token item(Stack const& stack, size_t const index)
            {
                return (stack.*&hack::c)[index];
            }
        };

        Stack temporaryStack;
        for (size_t i = 0; i < line->size(); ++i)
        {
            temporaryStack.push(hack::item(*line, i));
        }
        while (!temporaryStack.empty())
        {
            std::cout<<temporaryStack.top().value<<" ";
            temporaryStack.pop();
        }
        std::cout<<"\n";
    }

    std::cout<<"FUNCTIONS:\n";

    for (const auto &function : *functions)
    {
        auto funcName = *function->getChildrenPtr()->begin();
        std::cout<<funcName->getLabel().value<<" ";
    }
    std::cout<<"\n";

    return {heap, functions}; 
}

void runProgram(Memory mem)
{
    std::cout<<std::setw(29)<<std::setfill('*')<<std::right<<"INTER";
    std::cout<<std::setw(28)<<std::left<<"PRETER"<<"\n";

    Interpreter interpreter(mem);
    interpreter.run();

    interpreter.showVariables();
}
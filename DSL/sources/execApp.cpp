#include "../headers/execApp.h"

int execApp()
{
    std::ifstream in("../resources/program.dsl");

    V list      = *analyze(&in);

    bool parsed = list.empty() ? false : parse(&list) == 0;

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

    for (const auto &el : *tokenList)
    {
        std::cout<<std::setw(32)<<std::left<<el->value.c_str()<<":";
        std::cout<<std::setw(24)<<std::right<<el->type.c_str()<<"\n";
    }

    return tokenList;
}

int parse(V *tokenList)
{
    Parser parser(tokenList);
    return parser.lang();
}
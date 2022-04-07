#include "../headers/execApp.h"

int execApp()
{
    std::ifstream in("../resources/program.dsl");

    V list = *analyze(&in);

    parse(&list);

    return 0;
}

V * analyze(std::ifstream *input)
{
    Lexer lexer;

    V *tokenList;
    if (input->is_open())
    {
        tokenList = lexer.tokenize(input);
    }
    else 
    {
        std::cerr<<"Oops!"<<"\n";
    }
    input->close();

    if (!tokenList->empty())
    {
        for (const auto &el : *tokenList)
        {
            std::cout<<el->value<<" : "<<el->type<<"\n";
        }
    }

    return tokenList;
}

int parse(V *tokenList)
{
    Parser parser(tokenList);
    parser.syntaxCheck();

    return 0;
}
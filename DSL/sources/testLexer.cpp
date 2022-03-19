#include "../headers/testLexer.h"

int testLexer()
{
    Lexer lexer;

    std::ifstream in("../resources/Programm.txt");
    V list;
    if (in.is_open())
    {
        list = *lexer.tokenize(&in);
    }
    in.close();

    if (!list.empty())
    {
        for (const auto &el : list)
        {
            std::cout<<el->value<<" : "<<el->type<<std::endl;
        }
    }

    return 0;
}
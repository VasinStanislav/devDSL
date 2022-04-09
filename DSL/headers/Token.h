#ifndef TOKEN_H
#define TOKEN_H

#include <string>

struct Token
{
    std::string value;
    std::string type;
    unsigned int line;
};

#endif
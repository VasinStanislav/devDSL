#include "regex4vars.h"

bool isValidVariableName(std::string str)
{
    static const std::regex r(R"([a-zA-Z\_\♂\☼]{1}[0-9a-zA-Z\_]{0,30}[0-9a-z\_\♂\☼]{1})");
    return std::regex_match(str.data(), r);
}
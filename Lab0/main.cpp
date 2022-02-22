#include <iostream>
#include "regex4vars.h"

int main()
{
    std::cout<<"Введите строку:\n";

    std::string str;
    std::getline(std::cin, str);

    std::cout<<isValidVariableName(str)<<std::endl;

    return 0;
}
#ifndef EXECAPP_H
#define EXECAPP_H

#include "Lexer.h"
#include "Parser.h"
#include <fstream>
#include <iostream>


int execApp();

V* lexAnalysis(std::ifstream *);
int parse(V *);


#endif
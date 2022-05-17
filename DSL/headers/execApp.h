#ifndef EXECAPP_H
#define EXECAPP_H

#include "Lexer.h"
#include "Parser.h"
#include "StackMachine.h"
#include <fstream>
#include <iostream>
#include <iomanip>


int execApp();

V* analyze(std::ifstream *);
AST * parse(V *);
// testing
Content *getRPN(AST *);


#endif
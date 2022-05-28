# Non-terminals
lang -> (expr)*

expr -> (functionDef | functionCall | assignment | opIf | opFor | opWhile | opDoWhile)

functionDef -> def function lBracket (variable (argsSeparator variable)* )? rBracker block

functionCall -> function lBracket (value (argsSeparator (arithmeticExpression | conditionalExpression) )* )? rBracket separator?

block -> lBrace (functionDef | functionCall | assignment | opIf | opReturn | opWhile | opDoWhile | opFor | opBreak | opContinue)* rBrace

assignment -> variable assignOp (arithmeticExpression | conditionalExpression) separator?

allocation -> new constructor lBracket (value (argsSeparator (arithmeticExpression | conditionalExpression) )* )? rBracket separator?

arithmeticExpression -> (prefix? arithmeticBrackets postfix? | (prefix? arithmeticBrackets postfix? mathOp prefix? (value | functionCall | allocation) postfix?) | (prefix? arithmeticBrackets postfix? mathOp prefix? (value | functionCall | allocation) postfix? mathOp arithmeticExpression) ) | (prefix? (value | functionCall | allocation) postfix?) | (prefix? (value | functionCall | allocation) postfix? mathOp arithmeticExpression)

arithmeticBrackets -> lBracket arithmeticExpression rBracket

prefix -> unaryOp

postfix -> unaryOp

conditionalExpression -> (logicalNegation? conditionalBrackets | (logicalNegation? conditionalBrackets (comprOp | logicalOp) logicalNegation? (value | functionCall | allocation) ) | (logicalNegation? conditionalBrackets (comprOp | logicalOp) logicalNegation? (value | functionCall | allocation)  (comprOp | logicalOp) contidionalExpression) | (logicalNegation? (value | functionCall | allocation) (comprOp | logicalOp) conditionalExpression) | (logicalNegation? (value | functionCall | allocation))

conditionalBrackets -> lBracker conditionalExpression rBracket

opReturn -> return (arithmeticExpression | conditionalExpression)?

opBreak -> break separator?

opContinue -> continue separator?

opIf -> if lBracket conditionalExpression rBracket block opElif* opElse?

opElif -> elif lBracket conditionalExpression rBracket block

opElse -> else block

opDoWhile -> do block while lBracket conditionalExpression rBracket separator?

opWhile -> while lBracket conditionalExpression rBracket block 

opFor -> for lBracket assignment conditionalExpression separator assignment rBracket block

# Terminals

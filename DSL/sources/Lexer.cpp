#include "../headers/Lexer.h"

Lexer::Lexer()
{
    lexems.fill("VARIABLE",            (regex)(R"(^[a-zA-Z\_]{1}[0-9a-zA-Z\_]{0,31}$)"));
    lexems.fill("FUNCTION",            (regex)(R"(^[a-z\_]{1}[0-9a-zA-Z\_]{0,31}\(\)$)"));
    lexems.fill("INTEGER",             (regex)(R"(^0|([1-9][0-9]*)$)"));
    lexems.fill("STRING",              (regex)(R"(^\"[0-9a-zA-Z\*\\/&\_\.\,\;\\\!\?\- )\(]*\"$)"));
    lexems.fill("MATH_OPERATOR",       (regex)(R"(^[%+*\-\\]$)"));
    lexems.fill("ASSIGN_OPERATOR",     (regex)(R"(^(=)$)"));
    lexems.fill("COMPRASION_OPERATOR", (regex)(R"(^[<>!]|(>=)|(<=)|(==)$)"));
    lexems.fill("L_BRACKET",           (regex)(R"(^\($)"));
    lexems.fill("R_BRACKET",           (regex)(R"(^\)$)"));
    lexems.fill("L_BRACE",             (regex)(R"(^\{$)"));
    lexems.fill("R_BRACE",             (regex)(R"(^\}$)"));
    lexems.fill("ARG_SEPARATOR",       (regex)(R"(^\,$)"));
    lexems.fill("SEPARATOR",           (regex)(R"(^\;$)"));

    keyWords.fill("DEFINITION",        (regex)(R"(^(def)$)"));
    keyWords.fill("IF_KW",             (regex)(R"(^(if)$)"));
    keyWords.fill("ELSE_KW",           (regex)(R"(^(else)$)"));
    keyWords.fill("ELIF_KW",           (regex)(R"(^(elif)$)"));
    keyWords.fill("WHILE_KW",          (regex)(R"(^(while)$)"));
    keyWords.fill("FOR_KW",            (regex)(R"(^(for)$)"));
    keyWords.fill("RETURN",            (regex)(R"(^(return)$)"));
}

void MyMap::fill(string str, regex regx) { this->insert(std::make_pair(str, regx)); }

template <typename T>
string Lexer::getTocken(string str, T lexems)
{
    M::iterator it = lexems.begin();

    string tocken;
    for (; it != lexems.end(); it++)
    {
        if (std::regex_match(str.data(), it->second))
        {
            tocken = it->first;
        }
    }

    return tocken;
}

str_pair Lexer::nipOff(string line)
{
    string curStr = "";

    while(line[0] == ' ' or line[0] == '\t')
    {
        line = line.substr(1);
    }

    if (line[0] == '(' or line[0] == ')' or line[0] == '{' or line[0] == '}')
    {
        curStr = line[0];
        line = line.substr(1);
    }
    else if (line[0] == '"')
    {
        curStr = "\"";
        line = line.substr(1);
        curStr += line.substr(0, line.find("\"") + 1);
        line = line.substr(line.find("\"") + 1);
    }
    else 
    {
        int pos = line.find("(");

        if (pos < line.find(" "))
        {
            curStr = line.substr(0, pos);
            curStr.append("()");
            line = line.substr(pos);
        }
        else 
        {
            curStr = line[0];

            while (true)
            {
                if (this->getTocken(curStr, this->lexems) != "") 
                {
                    if (line.length() < 1) { break; }
                    line = line.substr(1);
                    curStr += line[0];
                }
                else 
                {
                    curStr.erase(curStr.length()-1);
                    break;
                }
            }

        }

    }

    return {curStr, line};
}

V * Lexer::tokenize(std::ifstream *input)
{
    V *tokenList = nullptr;
    
    string line = "", curStr = "";
    if (input)
    {
        tokenList = new V();
        unsigned int strNum = 1;

        while(std::getline(*input, line, '\n'))
        {

            str_pair lines = {curStr, line};
            while (lines.second != "")
            {
                lines       = this->nipOff(lines.second);
                string type = this->getTocken(lines.first, this->lexems);

                try 
                {
                    if (type != "")
                    {
                        if (type == "FUNCTION")
                        {
                            string curStrWithoutBrackets = lines.first.substr(0, 
                                                                            lines.first.length() - 2);
                            string tokenWithoutBrackets  = getTocken(curStrWithoutBrackets, keyWords);
                            if (tokenWithoutBrackets != ""  and tokenWithoutBrackets.substr(
                                (tokenWithoutBrackets.length() - 2)) == "KW") 
                            {
                                type    = tokenWithoutBrackets;
                            }
                            lines.first = curStrWithoutBrackets;
                        }
                        if (type == "VARIABLE")
                        {
                            string keyWDToken = getTocken(lines.first, keyWords);
                            if (keyWDToken != "") { type = keyWDToken; }
                        }

                        tokenList->push_back(new Token{lines.first, type, strNum});
                    }
                    else 
                    {
                        throw "in line " + std::to_string(strNum) + ": unknown lexeme" +
                        " lexical analysis has been stopped";
                    }
                }
                catch(const string &exception)
                {
                    std::cerr<<exception<<"\n";
                    for (size_t it = 0; it < tokenList->size(); ++it)
                    {
                        delete (*tokenList)[it];
                    }
                    tokenList->clear();
                    return tokenList;
                }
            }

            strNum++;
        }
    }

    return tokenList;
}
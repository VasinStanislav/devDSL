#include "../headers/Lexer.h"

Lexer::Lexer()
{
    this->lexems.insert(std::make_pair("VARIABLE", R"([a-zA-Z\_]{1}[0-9a-zA-Z\_]{0,31})"));
    this->lexems.insert(std::make_pair("FUNCTION", R"([a-z\_]{1}[0-9a-zA-Z\_]{0,31}[\s]*\([\s]*\))"));
    this->lexems.insert(std::make_pair("DIGIT", R"(0|([1-9][0-9]*))"));
    this->lexems.insert(std::make_pair("STRING", R"(\"[0-9a-zA-Z\*\\/&\_\.\,\;\\\!\?\-)\(]*\")"));
    this->lexems.insert(std::make_pair("OPERATOR", R"([%=+*\-\\<>!]|(>=)|(<=))"));
    this->lexems.insert(std::make_pair("L_BRACKET", R"(\()"));
    this->lexems.insert(std::make_pair("R_BRACKET", R"(\))"));
    this->lexems.insert(std::make_pair("L_BRACE", R"(\{)"));
    this->lexems.insert(std::make_pair("R_BRACE", R"(\})"));
    this->lexems.insert(std::make_pair("ARG_SEPARATOR", R"(\,)"));
    this->lexems.insert(std::make_pair("SEPARATOR", R"(\;)"));
    this->lexems.insert(std::make_pair("IF_KW", R"((if))"));
    this->lexems.insert(std::make_pair("ELSE_KW", R"((else))"));
    this->lexems.insert(std::make_pair("ELIF_KW", R"((elif))"));
    this->lexems.insert(std::make_pair("WHILE_KW", R"((while))"));
    this->lexems.insert(std::make_pair("FOR_KW", R"((for))"));

    this->keyWords.insert(std::make_pair("IF_KW", R"((if))"));
    this->keyWords.insert(std::make_pair("ELSE_KW", R"((else))"));
    this->keyWords.insert(std::make_pair("ELIF_KW", R"((elif))"));
    this->keyWords.insert(std::make_pair("WHILE_KW", R"((while))"));
    this->keyWords.insert(std::make_pair("FOR_KW", R"((for))"));
}

template <typename T>
std::string Lexer::getTocken(std::string str, T lexems)
{
    M::iterator it = lexems.begin();

    std::string tocken;
    for (; it != lexems.end(); it++)
    {
        if (std::regex_match(str.data(), it->second))
        {
            tocken = it->first;
        }
    }

    return tocken;
}

MM * Lexer::tokenize(std::ifstream *input)
{
    MM *tokenList = nullptr;
    
    std::string line = "";
    if (input)
    {
        tokenList = new MM();

        while(std::getline(*input, line, '\n'))
        {
            std::string newLine = "";
            for (int i = 0; i < line.length(); i++)
            {
                if (line[i] != ' ')
                {
                    newLine += line[i];
                }
            }
            line = newLine;
            while (line != "")
            {
                std::string curStr = "";

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
                    if (pos > -1 and pos < line.find(")") and pos < line.find(";")
                        and line.find(",") and line.find("{") and line.find("}"))
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

                std::string token = this->getTocken(curStr, this->lexems);
                if (token != "")
                {
                    if (token == "FUNCTION")
                    {
                        std::string curStrWithoutBrackets = 
                            curStr.substr(0, curStr.length() - 2);
                        std::string tokenWithoutBrackets = 
                            this->getTocken(curStrWithoutBrackets, this->keyWords);
                        if (tokenWithoutBrackets != ""  and 
                            tokenWithoutBrackets.substr(
                            (tokenWithoutBrackets.length() - 2)) == "KW") 
                        {
                            token = tokenWithoutBrackets;
                        }
                        curStr = curStrWithoutBrackets;
                    }
                    tokenList->insert(std::make_pair(token, curStr));
                }
            }
        }
    }

    return tokenList;
}
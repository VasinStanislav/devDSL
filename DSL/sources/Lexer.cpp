#include "../headers/Lexer.h"

Lexer::Lexer()
{
    this->lexems.insert(std::make_pair("VARIABLE", R"(^[a-zA-Z\_]{1}[0-9a-zA-Z\_]{0,31}$)"));
    this->lexems.insert(std::make_pair("FUNCTION", R"(^[a-z\_]{1}[0-9a-zA-Z\_]{0,31}\(\)$)"));
    this->lexems.insert(std::make_pair("INTEGER", R"(^0|([1-9][0-9]*)$)"));
    this->lexems.insert(std::make_pair("STRING", R"(^\"[0-9a-zA-Z\*\\/&\_\.\,\;\\\!\?\- )\(]*\"$)"));
    this->lexems.insert(std::make_pair("OPERATOR", R"(^[%=+*\-\\<>!]|(>=)|(<=)$)"));
    this->lexems.insert(std::make_pair("L_BRACKET", R"(^\($)"));
    this->lexems.insert(std::make_pair("R_BRACKET", R"(^\)$)"));
    this->lexems.insert(std::make_pair("L_BRACE", R"(^\{$)"));
    this->lexems.insert(std::make_pair("R_BRACE", R"(^\}$)"));
    this->lexems.insert(std::make_pair("ARG_SEPARATOR", R"(^\,$)"));
    //this->lexems.insert(std::make_pair("SEPARATOR", R"(^\;$)"));

    this->keyWords.insert(std::make_pair("IF_KW", R"(^(if)$)"));
    this->keyWords.insert(std::make_pair("ELSE_KW", R"(^(else)$)"));
    this->keyWords.insert(std::make_pair("ELIF_KW", R"(^(elif)$)"));
    this->keyWords.insert(std::make_pair("WHILE_KW", R"(^(while)$)"));
    this->keyWords.insert(std::make_pair("FOR_KW", R"(^(for)$)"));
    this->keyWords.insert(std::make_pair("RETURN", R"(^(return)$)"));
}

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

    return {curStr, line};
}

V * Lexer::tokenize(std::ifstream *input)
{
    V *tokenList = nullptr;
    
    string line = "", curStr = "", newLine = "";
    if (input)
    {
        tokenList = new V();
        unsigned int strNum = 1;

        while(std::getline(*input, line, '\n'))
        {
            bool inQuotes = false;
            for (int i = 0; i < line.length(); i++)
            {
                if (line[i] == '"')
                {
                    inQuotes = !inQuotes;
                }
                if (line[i] != ' ' or inQuotes)
                {
                    newLine += line[i];
                }
            }
            line = newLine;
            newLine = "";

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
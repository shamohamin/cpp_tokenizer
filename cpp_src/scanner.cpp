#include "include/scanner.hpp"
#include "string.h"
#include <fstream>
#include "regex"
#include "iterator"
#include "ctype.h"

Scanner::Scanner(std::string input_file, std::string output_file)
{
    this->input_filepath = input_file;
    this->output_filepath = output_file;
    this->tokeneRecognizer = new TokenRecognizer();
}

void Scanner::readingFile()
{
    std::string line;
    std::ifstream file(this->input_filepath);
    if (file.is_open())
    {
        while (getline(file, line))
        {
            // triming left and right side of line
            line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");
            this->srcLines.push_back(line);
        }

        file.close();
    }
    else
        std::cout << "Unable to open file";
}

void Scanner::scan()
{
    for (int i = 0; i < this->srcLines.size(); i++)
    {
        std::string line = this->srcLines[i];
        int start = 0;

        for (int j = 0; j < line.size() + 1; j++)
        {
            int end_line;
            char look_head = line[j];
            if (this->commentLogic(i, look_head, j, end_line))
            {
                i = end_line;
                break;
            }
            else if (this->tokeneRecognizer->isBrackets(std::string{look_head}))
            {
                Token token(BRACKETS, std::string{look_head});
                this->tokens[BRACKETS].push_back(std::make_pair(token, std::to_string(i)));
            }
            else if (this->tokeneRecognizer->isOperator(std::string{look_head}))
            {
                if ((look_head == '+' || look_head == '-') && isdigit(line[j + 1]))
                    continue;
                j = this->operatorLogic(j, look_head, line, i);
            }
            else if (this->tokeneRecognizer->isSeperator(std::string{look_head}))
            {
                Token token(SPERATOR, std::string{look_head});
                this->tokens[SPERATOR].push_back(std::make_pair(token, std::to_string(i)));
            }
            else if (look_head == '\"' || look_head == '\'') // strings characters
            {
                int start = i;
                int out = stringCharacterLogic(j + 1, look_head, line, start);
                if (out == -1)
                {
                    std::cout << "not closing tag for: " << look_head << std::endl;
                    exit(255);
                }
                else
                {
                    std::string temp = "";
                    if (start != i)
                    {
                        temp += line.substr(j, out - j + 1);
                        for (int k = i + 1; k < start; k++)
                            temp += this->srcLines[k];
                        temp += this->srcLines[start].substr(0, out + 1);
                        i = start;
                        Token token(LITERALS, temp);
                        this->tokens[LITERALS].push_back(std::make_pair(token, std::to_string(i)));
                        break;
                    }
                    else
                    {
                        temp += line.substr(j, out - j + 1);
                        Token token(LITERALS, temp);
                        this->tokens[LITERALS].push_back(std::make_pair(token, std::to_string(i)));
                        j = out;
                    }
                }
            }
            else if (isdigit(look_head))
            {
                int start = j;
                if (line[j - 1] == '+' || line[j - 1] == '-' || line[j - 1] == '.')
                    start = j - 1;
                int out = this->digitLogic(start, line.substr(start), i);
                std::cout << j << " " << out << "\n";
                if (out != -1)
                    j = start + out;
            }
        }
    }
    this->printTokens();
}

bool Scanner::commentLogic(int start_line, char temp, int start_comment_pos, int &end_line)
{
    end_line = start_line;
    if (temp == '/')
    {
        std::string line = this->srcLines[start_line];
        for (int i = start_comment_pos; i < line.size() + 1; i++)
        {
            std::string tmp = line.substr(start_comment_pos, i);
            if (this->tokeneRecognizer->isComment(tmp))
            {
                if (tmp.compare("/*") == 0)
                {
                    for (int j = start_line; j < this->srcLines.size(); j++)
                    {
                        std::string line = this->srcLines[j];
                        if (std::regex_search(line, std::regex("\\*/$")))
                        {
                            Token token(COMMENTS, "*/");
                            this->tokens[COMMENTS].push_back(std::make_pair(token, std::to_string(j)));
                            end_line = j;
                            break;
                        }
                    }
                }
                Token token(COMMENTS, tmp);
                this->tokens[COMMENTS].push_back(std::make_pair(token, std::to_string(start_line)));
                return true;
            }
        }
    }

    return false;
}

int Scanner::operatorLogic(int start_pos, char look_head, std::string line, int line_pos)
{
    int end_pos = start_pos;
    for (int i = start_pos; i < line.size(); i++)
    {
        std::string temp = line.substr(start_pos, i - start_pos);
        if (this->tokeneRecognizer->isOperator(temp))
            end_pos = i;
    }

    Token token(OPERATOR, line.substr(start_pos, end_pos - start_pos));
    this->tokens[OPERATOR].push_back(std::make_pair(token, std::to_string(line_pos)));

    return end_pos;
}

int Scanner::stringCharacterLogic(int start_pos, char look_head, std::string line, int &line_pos)
{
    int end_pos = start_pos;
    for (int i = start_pos; i < line.size(); i++)
    {
        // std::cout << "look head is: " << i + 1 << " size of line is " << line.size() << "\n";
        if (line.at(i) == look_head)
        {
            // if scape character then scape this.
            if (i - 1 >= 0 && line.at(i - 1) == '\\')
                continue;
            // std::cout << "lookhead is: " << look_head << " pos is:" << i << " start is: " << start_pos << "\n";
            return i;
        }
        else if (line.at(i) == '\\' && look_head != '\'') // this is multiple string line
        {
            if (i + 1 < line.size())
                if (!(line.at(i + 1) == '\n' || line.at(i + 1) == ' '))
                    continue;

            line_pos++;
            return this->stringCharacterLogic(0, look_head, this->srcLines[line_pos], line_pos);
        }
    }
    return -1;
}

int Scanner::digitLogic(int start_pos, std::string line, int &line_pos)
{
    std::smatch match;
    if (std::regex_search(line, match, DIGIT_PATTERN))
    {
        Token token(LITERALS, line.substr(match.position(0), match[0].length()));
        this->tokens[LITERALS].push_back(std::make_pair(token, std::to_string(line_pos)));
        std::cout << match[0] << " " << match.position(0) << "\n";
        return match.position(0) + match[0].length();
    }

    return -1;
}

void Scanner::printTokens()
{
    std::map<TokenType, std::vector<std::pair<Token, std::string> > >::iterator it;
    for (it = this->tokens.begin(); it != this->tokens.end(); it++)
    {

        std::cout << "Token Type: " << TokenTypeValues[it->first] << std::endl;
        for (int i = 0; i < it->second.size(); i++)
            std::cout << "\t" << ((it->second).at(i).first).getLexeme()
                      << " in line: " << (it->second).at(i).second << "\n";
    }
}

void Scanner::writingFile()
{
}

void Scanner::execute()
{
    this->readingFile();
    this->scan();
}

Scanner::~Scanner()
{
    delete this->tokeneRecognizer;
}

TokenRecognizer::TokenRecognizer()
{
}

const std::vector<std::string> TokenRecognizer::operators{"<", ">", ">=", "<=", "==", "=", "+", "-", "*", "/", "!", "!=", "%", "^", "&&", "&", "||", "?", ":"};
const std::vector<std::string> TokenRecognizer::seperators{",", ";"};
const std::vector<std::string> TokenRecognizer::keyboards{"int", "void", "float", "double", "class", "private", "uint", "public", "const", "static", "if", "for", "else", "else if", "auto", "while", "do", "map", "#include", "#define", "using", "namespace", "vector", "cout"};
const std::vector<std::string> TokenRecognizer::comments{"//", "/*"};
const std::vector<std::string> TokenRecognizer::brackets{"[", "]", "(", ")", "{", "}"};

bool TokenRecognizer::isComment(std::string temp)
{
    for (std::string val : TokenRecognizer::comments)
        if (temp.compare(val) == 0)
            return true;
    return false;
}

bool TokenRecognizer::isOperator(std::string temp)
{
    for (std::string val : TokenRecognizer::operators)
        if (temp.compare(val) == 0)
            return true;
    return false;
}

bool TokenRecognizer::isSeperator(std::string temp)
{
    for (std::string val : TokenRecognizer::seperators)
        if (temp.compare(val) == 0)
            return true;
    return false;
}

bool TokenRecognizer::isKeyboard(std::string temp)
{
    for (std::string val : TokenRecognizer::keyboards)
        if (temp.compare(val) == 0)
            return true;
    return false;
}

bool TokenRecognizer::isBrackets(std::string temp)
{
    for (std::string val : TokenRecognizer::brackets)
        if (temp.compare(val) == 0)
            return true;
    return false;
}

Token::Token(TokenType type, std::string value)
{
    this->lexeme = value;
    this->type = type;
}

std::string Token::getLexeme() const
{
    return this->lexeme;
}

TokenType Token::getType() const
{
    return this->type;
}
#include <iostream>
#include "include/scanner.hpp"
#include "string.h"
#include <fstream>
#include "regex"
#include "iterator"
#include "ctype.h"
#include "filesystem"
#include <sys/stat.h>
#include <sys/types.h>

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
                j = this->operatorLogic(j, look_head, line, i) - 1;
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
                        Token t(SPERATOR, this->srcLines[start].substr(out + 1, line.size() - (out + 1)));
                        this->tokens[SPERATOR].push_back(std::make_pair(t, std::to_string(start)));
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
                if (out != -1)
                    j = start + out - 1;
            }
            else if (isalpha(look_head) || look_head == '_' || look_head == '#')
            {
                if (look_head == '#')
                    j = this->identifierAndKeyboardsLogic(j + 1, line, i);
                else
                    j = this->identifierAndKeyboardsLogic(j, line, i);
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
    for (int i = start_pos; i < line.size() + 1; i++)
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

        if (line.at(i) == look_head)
        {
            // if scape character then scape this.
            if (i - 1 >= 0 && line.at(i - 1) == '\\')
                continue;
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

int Scanner::identifierAndKeyboardsLogic(int start_pos, std::string line, int line_pos)
{
    int start_pointer = start_pos;
    while (isalpha(line[start_pointer]) || isdigit(line[start_pointer]) || line[start_pointer] == '_')
        start_pointer++;

    std::string temp = line.substr(start_pos, start_pointer - start_pos);
    if (start_pos - 1 >= 0 && line[start_pos - 1] == '#')
        start_pos--;

    Token token;
    if (this->tokeneRecognizer->isKeyboard(temp))
        token = Token::newToken(KEYBOARD, temp);
    else
        token = Token::newToken(IDENTIFIER, temp);

    this->tokens[token.getType()].push_back(std::make_pair(token, std::to_string(line_pos)));

    return start_pointer - 1;
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
    namespace fs = std::filesystem;

    fs::path p = "outputs";
    if (!fs::exists(p))
        fs::create_directory(p);

    std::string output_path = "";
    if (OS == MAC || OS == UNIX || OS == LINUX)
        output_path += ("outputs/" + this->output_filepath);
    else if (OS == WIN32 || OS == WIN64)
        output_path += ("outputs\\" + this->output_filepath);
    else
    {
        std::cout << "YOUR OS DOESN'T support file system.";
        exit(255);
    }

    std::ofstream file;
    if (fs::exists(output_path))
        file.open(output_path, std::ofstream::trunc);
    else
        file.open(output_path, std::ios_base::out);

    if (file.is_open())
    {
        // file << "Token Types, lexemes && lines \n";
        file << "[ \n";
        for (auto const &[key, value] : this->tokens)
        {
            file << "  { \n";
            file << "\t \"TokenType\":  "
                 << "\"" << TokenTypeValues[key] << "\","
                 << "\n";
            file << "\t \"values\": [ \n";
            int counter = 0;
            for (auto val : value)
            {
                std::string lexeme = val.first.getLexeme();
                std::string generated_lexeme = "";
                for (int i = 0; i < lexeme.size(); i++)
                {
                    if (lexeme[i] == '\"')
                    {
                        if (lexeme[i - 1] == '\\')
                            continue;
                        generated_lexeme += "\\\"";
                        continue;
                    }
                    else if (lexeme[i] == '\\')
                    {
                        generated_lexeme += "\\\\";
                        continue;
                    }
                    generated_lexeme += lexeme[i];
                }
                // std::string lexeme = std::regex_replace(val.first.getLexeme(), std::regex("(?!\\\\)\""), "\\\"");
                // lexeme = std::regex_replace(lexeme, std::regex("\\"), "\\\\");

                file << "\t  { \n"
                     << "\t \t\"lexeme\": "
                     << "\"" << generated_lexeme << "\",\n"
                     << "\t \t\"line\": " << val.second << "\n";
                if (counter == value.size() - 1)
                    file << "\t  } \n";
                else
                    file << "\t  }, \n";

                counter++;
            }

            if (key == 6)
                file << " ]} \n";
            else
                file << "]}, \n";
        }
        file << "]";
        file.close();
    }
    else
    {
        std::cout << "couldn't open the file.\n";
        fprintf(stderr, "OUTPUT FILE WAS CORRUPT.");
        exit(255);
    }
}

void Scanner::execute()
{
    this->readingFile();
    this->scan();
    this->writingFile();
}

Scanner::~Scanner()
{
    delete this->tokeneRecognizer;
}

TokenRecognizer::TokenRecognizer()
{
}

const std::vector<std::string> TokenRecognizer::operators{
    "<",
    ">",
    ">=",
    "<=",
    "==",
    "=",
    "+=",
    "-=",
    "/=",
    "*=",
    "++",
    "--",
    "+",
    "-",
    "*",
    "/",
    "!",
    "!=",
    "%",
    "^",
    "&&",
    "&",
    "||",
    "?",
    ":"};
const std::vector<std::string> TokenRecognizer::seperators{",", ";"};
const std::vector<std::string> TokenRecognizer::keyboards{
    "int",
    "void",
    "float",
    "double",
    "class",
    "private",
    "uint",
    "public",
    "const",
    "static",
    "if",
    "for",
    "else",
    "else if",
    "auto",
    "while",
    "do",
    "include",
    "define",
    "using",
    "namespace",
    "return",
    "bool",
    "char",
    "volatile",
    "short",
    "friend",
    "catch",
    "enum",
    "throw",
    "false",
    "true",
    "register",
    "extern",
    "new",
    "asm",
    "protected",
    "sizeof",
    "struct",
    "static_cast",
    "virtual",
    "template",
    "switch",
    "case",
    "inline",
    "goto",
    "const_cast",
    "long",
    "signed",
    "typedef",
    "try",
    "continue",
    "break",
    "default",
    "delete"};
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

Token Token::newToken(TokenType type, std::string value)
{
    return Token(type, value);
}

std::string Token::getLexeme() const
{
    return this->lexeme;
}

TokenType Token::getType() const
{
    return this->type;
}
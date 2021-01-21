#include <iostream>
#include "string.h"
#include "vector"
#include "map"
#include "regex"

#ifndef _SCANNCER_H
#define _SCANNCER_H

enum TokenType
{
    SPERATOR,
    IDENTIFIER,
    OPERATOR,
    COMMENTS,
    KEYBOARD,
    LITERALS,
    BRACKETS
};

static const std::string TokenTypeValues[] =
    {
        std::string{"separator"},
        std::string{"identifier"},
        std::string{"operators"},
        std::string{"comments"},
        std::string{"keyboards"},
        std::string{"literals"},
        std::string{"brackets"}};

static const std::regex DIGIT_PATTERN("[+-]?[0-9]*\\.?[0-9]+([Ee][-+]?[0-9]+)?");

class Token
{
public:
    Token(TokenType type, std::string value);
    TokenType getType() const;
    std::string getLexeme() const;

private:
    TokenType type;
    std::string lexeme;
};

class TokenRecognizer
{
public:
    TokenRecognizer();
    bool isOperator(std::string);
    bool isSeperator(std::string);
    bool isKeyboard(std::string);
    bool isComment(std::string);
    bool isBrackets(std::string);
    bool isLiteral(std::string);

private:
    static const std::vector<std::string> operators;
    static const std::vector<std::string> seperators;
    static const std::vector<std::string> keyboards;
    static const std::vector<std::string> comments;
    static const std::vector<std::string> brackets;
};

class Scanner
{
public:
    Scanner(std::string input_file, std::string output_file);
    void execute();
    void printTokens();
    ~Scanner();

private:
    std::string input_filepath;
    std::string output_filepath;
    TokenRecognizer *tokeneRecognizer;
    std::vector<std::string> srcLines;
    std::map<TokenType, std::vector<std::pair<Token, std::string> > > tokens;
    void readingFile();
    void scan();
    void writingFile();
    bool commentLogic(int, char, int, int &);
    int operatorLogic(int, char, std::string, int);
    int stringCharacterLogic(int, char, std::string, int &);
    int digitLogic(int, std::string, int &);
};

#endif

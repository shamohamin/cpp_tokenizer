#include <iostream>
#include "string.h"
#include "vector"
#include "map"
#include "regex"

#ifdef _WIN32
        #define OS 1
    #elif _WIN64
        #define OS 2
    #elif __APPLE__ || __MACH__
        #define OS 0
    #elif __linux__
        #define OS 3
    #elif __FreeBSD__
        #define OS 5
    #elif __unix || __unix__
        #define OS 4
    #else
        #define OS -1
#endif


#ifndef _SCANNCER_H
#define _SCANNCER_H

#define MAC 0
#define WIN32 1
#define WIN64 2
#define LINUX 3
#define UNIX 4

enum TokenType
{
    SPERATOR,
    IDENTIFIER,
    OPERATOR,
    COMMENTS,
    KEYBOARD,
    LITERALS,
    BRACKETS,
    PREPROCESSORS
};

static const std::string TokenTypeValues[] =
    {
        std::string{"separator"},
        std::string{"identifier"},
        std::string{"operators"},
        std::string{"comments"},
        std::string{"keyboards"},
        std::string{"literals"},
        std::string{"brackets"},
        std::string{"preprocessor"}};

static const std::regex DIGIT_PATTERN("[+-]?[0-9]*\\.?[0-9]+([Ee][-+]?[0-9]+)?");

class Token
{
public:
    Token(TokenType type, std::string value);
    Token(){};
    TokenType getType() const;
    std::string getLexeme() const;
    static Token newToken(TokenType type, std::string value);

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
    bool isPreProcess(std::string);

private:
    static const std::vector<std::string> operators;
    static const std::vector<std::string> seperators;
    static const std::vector<std::string> keyboards;
    static const std::vector<std::string> comments;
    static const std::vector<std::string> brackets;
    static const std::vector<std::string> pre_process;
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
    int identifierAndKeyboardsLogic(int, std::string, int);
};

#endif

#include <iostream>
#include "include/scanner.hpp"

std::string copy_string(char *);

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        std::cout << "not enogh arguments please insert input path and output path\n";
        std::cout << "HELP \n";
        std::cout << "--------------------------------------------------";
        std::cout << "LIKE ./bin/main input1.txt output1.json \n"
                  << "OR \n";
        std::cout << "make run ARG1=input1.txt output1.json \n";
        exit(255);
    }
    std::string input_file = ""; 
    std::string output_file{argv[2]};

    if (OS == MAC || OS == UNIX || OS == LINUX)
        input_file += ("inputs/" + copy_string(argv[1]));
    else if (OS == WIN32 || OS == WIN64)
        input_file += ("input\\" + copy_string(argv[1]));
    else
    {
        std::cout << "YOUR OS DOESN'T support file system.";
        exit(255);
    }
    Scanner scanner(input_file, output_file);
    scanner.execute();
}

std::string copy_string(char *path)
{
    std::string temp = "";
    int i = 0;
    while (path[i])
    {
        temp += path[i];
        i++;
    }
    return temp;
}
#include <iostream>
#include "include/scanner.hpp"



int main()
{
    Scanner scanner("input.txt", "output.txt");
    scanner.execute();
}
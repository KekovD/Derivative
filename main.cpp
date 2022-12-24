#include "parser.h"
#include <iostream>
#include <string>
#include <windows.h>

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    std::string str;
    tree_element tree;

    std::cout << "Выражение (без пробелов):";
    std::cin >> str;

    tree.processing(str);

    return 0;
}

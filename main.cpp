#include <cstring>
#include <conio.h>
#include "parser.h"
#include <iostream>
#include <windows.h>

int main(int argc, char* argv[])
{
    SetConsoleOutputCP(CP_UTF8);

    char str[200] = {0};
    tree_elem tree;

    if(argc > 1)
        strcpy(str,argv[1]);
    else
    {
        std::cout << "Выражение (без пробелов):";
        std::cin >> str;
    }

    std::cout << "Исходное:";
    std::cout << str;
    tree.Processing(str);
    getch();

    return 0;
}
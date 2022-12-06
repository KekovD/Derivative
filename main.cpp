#include <conio.h>
#include "parser.h"
#include <iostream>
#include <windows.h>

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    char string[256] = {0};
    tree_element tree;

    std::cout << "Выражение (без пробелов):";
    std::cin >> string;

    tree.processing(string);
    getch();

    return 0;
}

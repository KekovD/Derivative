#ifndef DERIVATIVE_PARSER_H
#define DERIVATIVE_PARSER_H
#include <iostream>

enum functions
{
    notfunc, sin, arcsin, cos, arccos, tg, arctg, sqrt, exp, ln, log
};

class tree_element
{
public:
    tree_element* right,* left; // указатель на поддеревья
    char variable; //переменная
    char operation;   //+ - / * ^
    functions func; // notfunc, sin, arcsin, cos, arccos, tg, arctg, sqrt, exp, ln
    float number; // коэффициент
    bool complex_operation; // признак сложного оператора - *,/,^

    tree_element ();

    void processing (char*);  // преобразование выражения
    tree_element *build (char*, int it1 = 0, int it2 = 0, int complex_oper = 0); // дерево для исходного выражения
    tree_element *multiplication_and_division (char*, int, int); // умножение и деление элементов
    tree_element *create_leaf (char*, int, int); // создание лепестка дерева
    static bool is_digit (tree_element*); // проверка на число
    static bool is_zero (tree_element*); // проверка на нулевой элемент
    static bool is_one (tree_element*); // является число 1
    void ch_op (tree_element*); // инверсия оператора в элементе
    void change_operation (tree_element*); // замена оператора в дереве
    void calculate (tree_element*, char oper = '+'); // вычисление значения в лепестке
    void simplify (tree_element*); // упрощение элемента
    void differentiation (tree_element*, char); // дифференцирование элемента по ключу переменной
    void LRR (tree_element*, std::ostream&); // обход дерева
    void copy_element (tree_element*, tree_element*); // копирование элемента
    tree_element* copy_tree (tree_element*);  // копирование дерева
    void destroy_tree (tree_element*);  // удаление дерева
    int search_var (tree_element*, char); // поиск переменной (ключа) типа char
};

class buffer
{
private:
    char array[16];  // буфер
    int index;  // индекс текущего символа
public:

    buffer ();

    void add (char);  // добавляет символ в стек
    void clear () ;  // очистка стека
    char* get_string (char* s = nullptr);  // возвращает составленную строку
};

char* get_token (char*, int&); // Берет из строки токен с номером ind
functions is_function (char*);  // Проверяет, является ли строка функцией
char* get_function_name (functions);  // Возвращает имя функции по ее индексу
int analysis (char*, int&);  // Производит анализ правильности ввода
int char_to_int (char);  // Преобразует символ в число соответствующее символу
float get_number (char*, int&);  // Берет токен-число со строки
void ignore_brackets (char*, int &);  // Игнорирование выражение в скобках

#endif //DERIVATIVE_PARSER_H

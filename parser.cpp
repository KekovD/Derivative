#include "parser.h"
#include <cstring>
#include <cctype>
#include <iostream>

tree_element::tree_element ()
{
    right = nullptr;
    left = nullptr;
    variable = 0;
    operation = 0;
    func = notfunc;
    number = 1;
    complex_operation = false;
}

void tree_element::processing (char *string)  // Преобразование выражения
{
    char ch = 0;
    int string_index = 0;

    if (analysis (string,string_index))  // анализ строки на правильность ввода
    {
        tree_element* tree = nullptr;
        tree = build (string, 0, strlen (string) - 1);
        std::cout << "\nПеременная:";
        std::cin >> ch;

        differentiation (tree, ch);
        simplify (tree);
        change_operation (tree);
        calculate (tree);
        std::cout << "Результат:";
        LRR (tree, std::cout);
        return;
    }
    else
    {
        std::cout << "Ошибка...";
    }
}


tree_element *tree_element::build (char *string, int it1, int it2, int complex_oper)
{
    int current = it1;  // текущий узел
    tree_element* result = nullptr;

    if (string[current] == '(')
    {
        ignore_brackets (string, current);  // игнорируем то, что внутри их
    }

    if (current == it2 + 1)  // если номер символа равен второму слагаемому
    {
        result = build (string, it1 + 1, it2 - 1, 1);  // рекурсия по слагаемым
    }
    else  // поиск первого вхождения + - ( )
    {
        Repeat:

        while (current < it2 && string[current] != '+' && string[current] != '-'
            && string[current] != '(' && string[current] != ')')
        {
            current++;
        }

        if (current == it1 && string[current] == '+' || string[current] == '-')
        {
            current++;
            goto Repeat;  // возвращаемся на начало проверки
        }

        if (string[current] == '(')
        {
            ignore_brackets (string, current);
            goto Repeat; // проверяем следующий
        }

        if (current == it2 + 1)  // символ равен второму слагаемому
        {
            current--;
        }

        if (current != it2 && string[current] != ')')  // есть плюс или минус и нет скобки
        {
            result = new tree_element;  // создаем новый элемент

            if (string[current] == '-')
            {
                result->operation = '-';
            }

            if (string[current] == '+')
            {
                result->operation = '+';
            }

            result->complex_operation = complex_oper;  // то, что разбивается на произведения слева от текущего
            result->left = build (string, it1, current - 1);   // строим правую
            result->right = build (string, current + 1, it2);  // и левую ветвь
        }
        else  // если нет + и -
        {
            result = multiplication_and_division (string, it1, it2);  // разбиваем на произведения
        }
    }
    return result;
}

tree_element* tree_element::multiplication_and_division (char* string, int it1, int it2)
{
    int current = it1;

    Repeat_1:  // аналогично поиску + и -, но сначала ищем * и /, а потом ^

    while (current < it2 && string[current] != '*' && string[current] != '/' && string[current] != '(')  // поиск * и /
    {
        current++;
    }

    if (string[current] == '(')
    {
        ignore_brackets (string, current);
        current--;
        goto Repeat_1;
    }

    if(current == it2)  // поиск ^
    {
        current = it1;

        Repeat_2:

        while (current < it2 && string[current] != '^' && string[current] != '(')
        {
            current++;
        }

        if(string[current] == '(')
        {
            ignore_brackets (string, current);
            current--;
            goto Repeat_2;
        }
    }

    tree_element* result = nullptr;

    if (current != it2)  // текущая операция не равна второму слагаемому
    {
        result = new tree_element;

        if (string[current] == '*')
        {
            result->operation = '*';
        }
        else if (string[current] == '/')
        {
            result->operation = '/';
        }
        else if (string[current] == '^')
        {
            result->operation = '^';
        }

        result->left = build (string, it1, current - 1);  // строим левую
        result->right = build (string, current + 1, it2); // и правую ветвь
    }
    else  // если операция не найдена
    {
        result = create_leaf (string, it1, it2);
    }

    return result;
}

tree_element *tree_element::create_leaf (char* string, int it1, int it2)
{
    auto result = new tree_element;
    int current = it1;  // символ является первым слагаемым

    if(isdigit (string[it1]) || string[current] == '+' || string[current] == '-')  // если это число, + или -
    {
        result->number = get_number(string,current);  // записываем значение
    }

    if(isalpha (string[current]))  // если это символ
    {
        if(isalpha (string[current + 1]))  // если следующий символ
        {
            char* temp = nullptr;
            temp = get_token (string,current); // берём токен

            if (temp)
            {
                result->func = is_function (temp); // записываем найденную функцию
                delete temp;
            }

            result->left = build (string, current + 1, it2 - 1);  // строим поддерево аргумента
        }
        else
        {
            result->variable = string[current];  // записываем переменную
        }
    }

    return result;
}

void tree_element::LRR (tree_element *element, std::ostream &f)
{
    if (element)
    {
        if (element->func || element->variable)  // это функция или переменная
        {
            if (element->number != 1)  // это коэффициент и он не равен 1
            {
                f << element->number;
            }
        }
        else if (!element->operation)  // не функция или символ
        {
            f << element->number;
        }

        if (element->variable || element->operation)  // в узле знак или переменная
        {
            if (element->left)  // существует левая ветвь
            {
                if (element->operation)  // существует оператор
                {
                    if (((element->operation == '*' || element->operation == '/') && (element->left->operation == '+'
                        || element->left->operation == '-' || element->left->number < 0) )
                        || (element->operation == '^' && element->left->operation))
                    {
                        f << '(';
                        LRR(element->left, f); // вывод левой ветви
                        f << ')';
                    }
                    else
                    {
                        LRR (element->left, f);
                    }
                }
                else
                {
                    LRR (element->left, f);
                }
            }

            if (element->operation)  // если оператор
            {
                f << element->operation;
            }
            else  // если символ
            {
                f << element->variable;
            }

            if (element->right)  // аналогично правой
            {
                if(element->operation)
                {
                    if ((element->operation == '/' || (element->operation == '*' && (element->right->operation == '+'
                        || element->right->operation == '-' || element->right->number < 0)))
                        || (element->operation == '^' && element->right->operation))
                    {
                        f << '(';
                        LRR (element->right, f);
                        f << ')';
                    }
                    else
                    {
                        LRR (element->right, f);
                    }
                }
                else
                {
                    LRR(element->right, f);
                }
            }
        }
        else if (element->func)  // элемент - функция и не имеет ветвей
        {
            f << get_function_name (element->func); // выводим имя функции
            f << "(";
            LRR (element->left, f);  // выводим левое поддерево
            f << ")";
        }
    }
}

void tree_element::copy_element (tree_element *destination, tree_element *source)
{
    if (source && destination) // передатчик и приемник существуют
    {
        destination->variable = source->variable;
        destination->operation = source->operation;
        destination->func = source->func;
        destination->number = source->number;
        destination->complex_operation = source->complex_operation;
        destination->right = source->right;
        destination->left = source->left;
    }
}

tree_element* tree_element::copy_tree(tree_element* source)
{
    tree_element* temp = nullptr;

    if (source)  // передатчик существует
    {
        temp = new tree_element;
        temp->variable = source->variable;
        temp->operation = source->operation;
        temp->func = source->func;
        temp->number = source->number;
        temp->complex_operation = source->complex_operation;
        temp->right = copy_tree(source->right);
        temp->left = copy_tree(source->left);
    }

    return temp;
}

void tree_element::destroy_tree (tree_element * element)
{
    if(element)
    {
        destroy_tree (element->left);
        destroy_tree (element->right);
        delete element;
    }
}

int tree_element::search_var (tree_element* element, char ch)
{
    if (element)
    {
        return ((element->variable == ch) || search_var (element->left, ch)
                || search_var (element->right, ch));  // Вернуть символ или продолжать проверять
    }

    else return 0;
}

bool tree_element::is_digit (tree_element* element)
{
    if (element && !element->func && !element->variable && !element->operation)
    {
        return true;
    }

    return false;
}

bool tree_element::is_zero (tree_element* element)
{
    if(element && !element->func && !element->variable && !element->operation && !element->number)
    {
        return true;
    }

    return false;
}

bool tree_element::is_one (tree_element* element)
{
    if(element && !element->func && !element->variable && !element->operation && element->number == 1)
    {
        return true;
    }

    return false;
}

void tree_element::ch_op (tree_element* element)
{
    if (element)
    {
        if (element->operation == '+')
        {
            element->operation = '-';
        }
        else if (element->operation == '-')
        {
            element->operation = '+';
        }

        ch_op (element->left);
        ch_op (element->right);
    }
}

void tree_element::change_operation (tree_element* element)
{
    if (element)
    {
        if (element->operation == '-')
        {
            if (element->right->operation && element->right->complex_operation)  // в правом поддереве сложный оператор
            {
                ch_op (element->right);  // инвертировать операторы поддерева
            }

            if (element->right->operation == '+')
            {
                if (element->right->left && element->right->left->operation && element->right->left->complex_operation)
                    // в правом поддереве существует левая ветвь и она является оператором
                {
                    ch_op (element->right->left);  // Инвертировать операторы
                }
            }
        }

        change_operation (element->left);
        change_operation (element->right);
    }
}

void tree_element::calculate (tree_element* element, char oper)
{
    if (element)
    {
        if (element->operation)  // является операцией
        {
            calculate (element->left, oper);
            calculate (element->right, element->operation);

            if (is_digit (element->left) && is_digit (element->right))  // если являются числами
            {
                if(element->operation == '+' || element->operation == '-')
                {
                    switch (element->operation)  // вычисляем значения
                    {
                        case '+':
                            element->number = element->left->number + element->right->number;

                        case '-':
                            element->number = element->left->number + element->right->number;
                    }

                    element->operation = 0;  // обнуляем операции и удаляем ветви
                    element->complex_operation = 0;
                    delete element->left;
                    delete element->right;
                    element->left = nullptr;
                    element->right = nullptr;
                }
            }
        }
    }
}

void tree_element::simplify (tree_element* element)
{
    if (element)
    {
        simplify (element->left);
        simplify (element->right);

        label:

        if (element->operation)  // есть операция
        {
            if (is_zero (element->left) || is_zero (element->right))  // левая или правая ветвь равны 0
            {
                if (element->operation == '*' || element->operation == '/' && is_zero (element->left))  // + или - и левая == 0
                {
                    destroy_tree (element->left);
                    destroy_tree (element->right);
                    element->left = nullptr;
                    element->right = nullptr;
                    element->operation = 0;
                    element->number = 0;
                }
                else if (element->operation == '+' || element->operation == '-')  // + или -
                {
                    if(is_zero (element->left))  // левая равна 0
                    {
                        if(element->operation == '+')
                        {
                            destroy_tree (element->left);
                            tree_element* temp = element->right;  // создаем временную переменную, содержащую правую ветвь
                            copy_element (element, element->right);  // копируем содержимое элемента в его правую ветвь

                            if (temp)
                            {
                                delete temp;
                            }
                        }
                        else if (element->operation == '-')
                        {
                            element->operation = '*';  // установить оператор *
                            element->left->number = -1;  // установить значение левой ветви
                            goto label;  // проверить сначала
                        }
                    }
                    else if (is_zero (element->right))  // аналогично левой части
                    {
                        destroy_tree (element->right);
                        tree_element* temp = element->left;
                        copy_element (element, element->left);

                        if (temp)
                        {
                            delete temp;
                        }
                    }
                }
                else if (element->operation == '^')  // x^0 = 1;  0^a = 0;
                {
                    destroy_tree (element->left); // удаляем ветви и удаляем операцию
                    element->left = nullptr;
                    destroy_tree (element->right);
                    element->right = nullptr;
                    element->operation = 0;

                    if(is_zero (element->right))  // присваиваем результат
                    {
                        element->number = 1;
                    }
                    else
                    {
                        element->number = 0;
                    }
                }
            }
            else if (is_digit (element->left) && is_digit (element->right) && element->operation != '+'
                && element->operation != '-')  // правая и левая не равны 0 и операция не + и -
            {
                switch (element->operation)
                {
                    case '*':
                        element->number = element->left->number * element->right->number;
                        break;
                    case '/':
                        element->number = element->left->number / element->right->number;
                        break;
                    case '+':
                        element->number = element->left->number + element->right->number;
                        break;
                    case '-':
                        element->number = element->left->number - element->right->number;
                }

                element->operation = 0;  // обнуляем операцию и удаляем ветви
                delete (element->left);
                delete (element->right);
                element->left = nullptr;
                element->right = nullptr;
            }

            if (element->operation == '*')
            {
                if (is_one (element->left))
                {
                    destroy_tree (element->left);
                    tree_element* temp = element->right;  // копировать во временное дерево правую ветвь
                    copy_element (element, element->right); // копировать дерево в его правую ветвь

                    if (temp)
                    {
                        delete temp;
                    }
                }
                else if (is_one (element->right))  // аналогично левой
                {
                    destroy_tree (element->right);
                    tree_element* temp = element->left;
                    copy_element (element, element->left);

                    if (temp)
                    {
                        delete temp;
                    }
                }
            }

            if (element->operation == '^' && is_one (element->right))  // операция ^ и правая ветвь равна 1
            {
                delete element->right;  // удаляем правую ветвь
                tree_element* tmp = element->left;  // копируем левую во временную переменную
                copy_element (element, element->left); // копируем дерево в его левую ветвь
                delete tmp;
            }
        }
    }
}

void tree_element::differentiation (tree_element* element, char ch)
{
    if(element)
    {
        if(search_var(element, ch))  // в выражении есть переменная ch
        {
            if(element->operation)  // элемент является операцией
            {
                switch(element->operation)
                {
                    case '+':  // (a+b)'= a'+ b'
                    case '-':  // (a-b)'= a'- b'
                        differentiation (element->left, ch);
                        differentiation (element->right, ch);
                        break;
                    case '*':  // (ab)'= a'b + ab'
                    {
                        element->operation = '+'; // оператор результата
                        element->complex_operation = true; // признак сложного оператора

                        auto temp1 = new tree_element;  // создаем новый элемент
                        temp1->operation = '*';  // присваиваем операцию
                        temp1->left = copy_tree (element->left);  // копируем в него левую ветвь
                        differentiation (temp1->left, ch);  // дифференцируем левое поддерево
                        temp1->right = copy_tree (element->right);  // копируем обратно

                        auto *temp2 = new tree_element;  // аналогично левой
                        temp2->operation = '*';
                        temp2->left = element->left;
                        temp2->right = element->right;
                        differentiation (temp2->right, ch);

                        element->left = temp1; // записываем в дерево
                        element->right = temp2;
                        break;
                    }
                    case '/':  // (a/b)'= (a'b - ab')/b^2
                    {
                        element->operation = '/';  // аналогично *

                        auto *temp1 = new tree_element;
                        temp1->operation = '^';
                        temp1->left = copy_tree (element->right);
                        temp1->right = new tree_element;
                        temp1->right->number = 2;

                        auto *temp2 = new tree_element;
                        temp2->operation = '*';
                        temp2->left = copy_tree (element->left);
                        differentiation (temp2->left, ch);
                        temp2->right = copy_tree (element->right);

                        auto *temp3 = new tree_element;
                        temp3->operation = '*';
                        temp3->left = element->left;
                        temp3->right = element->right;
                        differentiation (temp3->right, ch);

                        auto *temp4 = new tree_element;
                        temp4->operation = '-';
                        temp4->complex_operation = 1;
                        temp4->left = temp2;
                        temp4->right = temp3;

                        element->left = temp4;
                        element->right = temp1;
                        break;
                    }
                    case '^':  // аналогично * и /, но с проверкой на сложную степень
                    {
                        element->operation = '*';
                        tree_element *temp1;

                        if (is_digit (element->right) && element->right->number != 1)  // x^a = a*x^(a-1)
                        {
                            temp1 = new tree_element;
                            temp1->operation = '^';
                            temp1->left = element->left;
                            temp1->right = element->right;
                            element->left = new tree_element;
                            element->left->number = element->right->number;
                            temp1->right->number -= 1;
                            element->right = temp1;
                        }
                        else  // (a^b)' = a^b*(b'*ln(a) + b*a'/a)
                        {
                            temp1 = new tree_element;
                            temp1->operation = '^';
                            temp1->left = element->left;
                            temp1->right = element->right;
                            element->left = temp1;

                            element->right = new tree_element;
                            element->right->operation = '+';
                            element->complex_operation = true;

                            temp1 = new tree_element;
                            temp1->operation = '*';
                            temp1->left = copy_tree(element->left->right);
                            differentiation (temp1->left, ch);
                            temp1->right = new tree_element;
                            temp1->right->func = ln;
                            temp1->right->left = copy_tree(element->left->left);

                            element->right->left = temp1;

                            temp1 = new tree_element;
                            temp1->operation = '*';
                            temp1->left = copy_tree(element->left->right);
                            temp1->right = new tree_element;
                            temp1->right->operation = '/';
                            temp1->right->left = copy_tree(element->left->left);
                            differentiation (temp1->right->left, ch);
                            temp1->right->right = copy_tree(element->left->left);

                            element->right->right = temp1;
                        }
                    }
                }
            }
            else if (element->variable)  // если символ
            {
                element->variable = 0; // записываем значения
                element->number = 1;
            }
            else if (element->func)  // если функция
            {
                auto temp1 = new tree_element;  // (f(g(x)))' = g'(x)*f'(g(x))
                copy_element (temp1, element);
                element->func = notfunc;
                element->operation = '*';
                element->number = 1;

                element->left = copy_tree(temp1->left);
                differentiation (element->left, ch);

                if (temp1->func == sin)  // sin'= cos
                {
                    element->right = temp1;
                    element->right->func = cos;
                }
                else if (temp1->func == cos)  // cos'= -sin
                {
                    element->right = new tree_element;
                    element->right->operation = '*';
                    element->right->left = new tree_element;
                    element->right->left->number = -1;
                    element->right->right = temp1;
                    element->right->right->func = sin;
                }
                else if (temp1->func == arcsin)  // arcsin'(x)= 1/sqrt(1-x^2)
                {
                    element->right = new tree_element;
                    element->right->operation = '/';
                    element->right->left = new tree_element;

                    if(temp1->func == arccos)
                    {
                        element->right->left->number = 1;
                    }

                    element->right->right = new tree_element;
                    element->right->right->func = sqrt;

                    auto temp2 = new tree_element;
                    temp2->operation = '-';
                    temp2->left = new tree_element;
                    temp2->right = new tree_element;
                    temp2->right->operation = '^';
                    temp2->right->left = temp1->left;
                    temp2->right->right = new tree_element;
                    temp2->right->right->number = 2;

                    element->right->right->left = temp2;
                }
                else if (temp1->func == arccos)  // arccos'(x)= -1/sqrt(1-x^2)
                {
                    element->right = new tree_element;
                    element->right->operation = '/';
                    element->right->left = new tree_element;

                    if(temp1->func == arccos)
                    {
                        element->right->left->number =-1;
                    }

                    element->right->right = new tree_element;
                    element->right->right->func = sqrt;

                    auto temp2 = new tree_element;
                    temp2->operation = '-';
                    temp2->left = new tree_element;
                    temp2->right = new tree_element;
                    temp2->right->operation = '^';
                    temp2->right->left = temp1->left;
                    temp2->right->right = new tree_element;
                    temp2->right->right->number = 2;

                    element->right->right->left = temp2;
                }
                else if (temp1->func == tg)  // tg'(x)= 1 + (tg(x))^2
                {
                    element->right = new tree_element;
                    element->right->operation = '+';
                    element->right->left = new tree_element;
                    element->right->right = new tree_element;
                    element->right->right->operation = '^';
                    element->right->right->left = temp1;
                    element->right->right->right = new tree_element;
                    element->right->right->right->number = 2;
                }
                else if (temp1->func == arctg)  // arctg'(x) = 1/(1+x^2)
                {
                    element->right = new tree_element;
                    element->right->operation = '/';
                    element->right->left = new tree_element;
                    element->right->right = new tree_element;
                    element->right->right->operation = '+';
                    element->right->right->left = new tree_element;
                    element->right->right->right = new tree_element;
                    element->right->right->right->operation = '^';
                    element->right->right->right->left = temp1->left;
                    element->right->right->right->right = new tree_element;
                    element->right->right->right->right->number = 2;
                }
                else if (temp1->func == sqrt)  // sqrt'(x) = (x^(1/2))'= 1/(2*sqrt(x))
                {
                    element->right = new tree_element;
                    element->right->operation = '/';
                    element->right->left = new tree_element;
                    element->right->right = new tree_element;
                    element->right->right->operation = '*';
                    element->right->right->left = new tree_element;
                    element->right->right->left->number = 2;
                    element->right->right->right = temp1;
                }
                else if (temp1->func == exp)  // (e^x)' = e^x
                {
                    element->right = temp1;
                }
                else if (temp1->func == ln)  // ln'(x) = 1/x
                {
                    element->right = new tree_element;
                    element->right->operation = '/';
                    element->right->left = new tree_element;
                    element->right->right = temp1->left;
                }
            }
        }
        else  // нет переменной ch
        {
            element->func = notfunc;
            element->operation = 0;
            element->variable = 0;
            element->number = 0;
            destroy_tree (element->left);
            destroy_tree (element->right);
            element->left = nullptr;
            element->right = nullptr;
        }
    }
}

buffer::buffer ()
{
    clear();
}

void buffer::add (char ch)
{
    array[index] = ch;
    index++;
    array[index] = 0;  // конец строки
}

void buffer::clear ()
{
    index = 0;
    array[index] = 0;
}

char* buffer::get_string (char* string)
{
    if(string)
    {
        strcpy(string, array);  // копируем стек в сроку
    }
    return array;
}

char* get_token(char* string, int &index)
{
    char * result = new char [16];

    if (isalpha (string[index]) && isalpha (string[index + 1]))  // собираем последовательность букв
    {
        buffer buf;

        while (isalpha (string[index])) // Если символ строки - символ
        {
            buf.add (string[index]);  // добавляем в буфер
            index++;  // проверяем следующий
        }
        strcpy (result,buf.get_string());  // копируем буфер
    }

    return result;
}

functions is_function (char* str)  // для ввода
{
    functions res = notfunc;
    if(!stricmp(str,"sin")) res = sin;
    else if (!stricmp(str,"asin")) res = arcsin;
    else if (!stricmp(str,"cos")) res = cos;
    else if (!stricmp(str,"acos")) res = arccos;
    else if (!stricmp(str,"tg")) res = tg;
    else if (!stricmp(str,"arctg")) res = arctg;
    else if (!stricmp(str,"sqrt")) res = sqrt;
    else if (!stricmp(str,"e")) res = exp;
    else if (!stricmp(str,"ln")) res = ln;
    else if (!stricmp(str,"log")) res = log;
    return res;
}

char* get_function_name (functions index)  // для вывода
{
    char* result= nullptr;

    switch (index)
    {
        case 1: result = (char*) "sin"; break;
        case 2: result = (char*) "asin"; break;
        case 3: result = (char*) "cos"; break;
        case 4: result = (char*) "acos"; break;
        case 5: result = (char*) "tg"; break;
        case 6: result = (char*) "arctg"; break;
        case 7: result = (char*) "sqrt"; break;
        case 8: result = (char*) "exp"; break;
        case 9: result = (char*) "ln"; break;
        case 10: result = (char*) "log"; break;
        default: result = (char*) "Error";
    }

    return result;
}

int analysis (char *string, int &index)
{
    bool result = true, bracket = false;

    if (string[index] == '(')
    {
        bracket = true;
        index++;
    }

    while (string[index] && result && string[index] != ')')  // существует символ и это не ')'
    {
        if (string[index] == '(')
        {
            result = analysis (string,index);
        }
        else
        {
            if (isalpha (string[index]) && isalpha (string[index + 1])) // составляем последовательность символов
            {
                buffer buf;

                while (isalpha(string[index]))
                {
                    buf.add (string[index]);
                    index++;
                }

                index--;
                result = (result && is_function (buf.get_string())); // проверяем является ли функцией
            }

            index++;
        }
    }

    if (bracket)
    {
        if(string[index] != ')')  // если нет закрывающей
        {
            result = false;
        }

        index++;
    }
    else if(string[index] == ')') // если была только закрывающая
    {
        result = false;
    }

    return result;
}

int char_to_int (char ch)
{
    return ch - 48;
}

float get_number(char *string, int &current)
{
    int mult1 = 10;
    float mult2 = 1;
    float result = 0;
    int sign = 1; // признак знака + или -

    if (string[current] == '+' || string[current]=='-')
    {
        if (string[current] == '-')
        {
            sign -= 2;
        }

        current++; // следующий символ
    }

    while (isdigit (string[current]) || string[current] == '.')
    {
        if (string[current] == '.')
        {
            mult1 = 1; // признак вещественного
            mult2 = 1;
        }
        else // число не вещественое
        {
            result = result * mult1 + char_to_int (string[current]) * mult2;
        }

        if (mult1 < 10)  // число вещественное
        {
            mult2 *= 0.1;
        }

        current++;
    }

    return result * sign;
}

void ignore_brackets(char* string, int &current)
{
    if(string[current] == '(')
    {
        while(string[current] != ')')
        {
            current++;

            if(string[current] == '(')
            {
                ignore_brackets (string,current);
            }
        }

        current++;  // возвращает следующий символ после скобок
    }
}



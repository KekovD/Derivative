#include "parser.h"
#include <cstring>
#include <cctype>
#include <iostream>

tree_elem::tree_elem ()
{
    right = nullptr;
    left = nullptr;
    symbol = 0;
    oper = 0;
    func = notfunc;
    number = 1;
    complex_oper = 0;
}

void tree_elem::Processing (char *str) // Процесс преобразования выражения
{
    char ch = 0;
    int str_index = 0;

    if(analysis (str,str_index))
    {
        tree_elem* my_tree = nullptr;
        my_tree = build_tree (str, 0, strlen (str) - 1);
        std::cout << "\nПеременная:";
        std::cin >> ch;

        Diff (my_tree, ch);
        Simplify (my_tree);
        change_oper (my_tree);
        Calc (my_tree);
        std::cout << "Результат:";
        LRR (my_tree, std::cout);
        return;
    }
    else
    {
        std::cout << "Ошибка...";
    }
}


tree_elem *tree_elem::build_tree (char *str, int ind1, int ind2, int complex_oper)
{
    int cur = ind1;
    tree_elem* res = nullptr;

    if (str[cur] == '(')
    {
        ignore_brackets (str, cur);
    }

    if (cur == ind2 + 1)
    {
        res = build_tree (str, ind1 + 1, ind2 - 1, 1);
    }
    else
    {
        Repeat:

        while (cur < ind2 && str[cur] != '+' && str[cur] != '-' && str[cur] != '(' && str[cur] != ')')
        {
            cur++;
        }

        if (cur == ind1 && str[cur] == '+' || str[cur] == '-')
        {
            cur++;
            goto Repeat;
        }

        if (str[cur] == '(')
        {
            ignore_brackets (str, cur);
            goto Repeat;
        }

        if (cur == ind2 + 1)
        {
            cur--;
        }

        if (cur != ind2 && str[cur] != ')')
        {
            res = new tree_elem;

            if (str[cur] == '-')
            {
                res->oper = '-';
            }

            if (str[cur] == '+')
            {
                res->oper = '+';
            }

            res->complex_oper = complex_oper;
            res->left = build_tree (str, ind1, cur - 1);
            res->right = build_tree (str, cur + 1, ind2);
        }
        else
        {
            res = mul_n_div (str, ind1, ind2);
        }
    }
    return res;
}

tree_elem* tree_elem::mul_n_div (char* str, int ind1, int ind2)
{
    int cur = ind1;

    Repeat_1:

    while (cur < ind2 && str[cur] != '*' && str[cur] != '/' && str[cur] != '(')
    {
        cur++;
    }

    if (str[cur] == '(')
    {
        ignore_brackets (str, cur);
        cur--;
        goto Repeat_1;
    }

    if(cur == ind2)
    {
        cur = ind1;

        Repeat_2:

        while (cur < ind2 && str[cur] != '^' && str[cur] != '(')
        {
            cur++;
        }

        if(str[cur] == '(')
        {
            ignore_brackets (str, cur);
            cur--;
            goto Repeat_2;
        }
    }

    tree_elem* res = nullptr;

    if (cur != ind2)
    {
        res = new tree_elem;

        if (str[cur] == '*')
        {
            res->oper = '*';
        }
        else if (str[cur] == '/')
        {
            res->oper = '/';
        }
        else if (str[cur] == '^')
        {
            res->oper = '^';
        }

        res->left = build_tree (str, ind1, cur - 1);
        res->right = build_tree (str, cur + 1, ind2);
    }
    else
    {
        res = create_leaf (str, ind1, ind2);
    }

    return res;
}

tree_elem *tree_elem::create_leaf (char* str, int ind1, int ind2)
{
    auto res = new tree_elem;
    int cur = ind1;

    if(isdigit (str[ind1]) || str[cur] == '+' || str[cur] == '-')
    {
        res->number = get_number(str,cur);
    }

    if(isalpha (str[cur]))
    {
        if(isalpha (str[cur+1]))
        {
            char* temp = nullptr;
            temp = get_token (str,cur);

            if (temp)
            {
                res->func = isfunc (temp);
                delete temp;
            }

            res->left = build_tree (str, cur + 1, ind2 - 1);
        }
        else
        {
            res->symbol = str[cur];
        }
    }

    return res;
}

void tree_elem::LRR (tree_elem *elem, std::ostream &f)
{
    if (elem)
    {
        if (elem->func || elem->symbol)
        {
            if (elem->number!=1)
            {
                f << elem->number;
            }
        }
        else if (!elem->oper)
        {
            f << elem->number;
        }

        if (elem->symbol || elem->oper)
        {
            if (elem->left)
            {
                if (elem->oper)
                {
                    if (((elem->oper == '*' || elem->oper == '/') && (elem->left->oper == '+' || elem->left->oper == '-'
                             || elem->left->number < 0) ) || (elem->oper == '^' && elem->left->oper))
                    {
                        f << '(';
                        LRR(elem->left, f);
                        f << ')';
                    }
                    else
                    {
                        LRR (elem->left, f);
                    }
                }
                else
                {
                    LRR (elem->left, f);
                }
            }

            if (elem->oper)
            {
                f << elem->oper;
            }
            else
            {
                f << elem->symbol;
            }

            if (elem->right)
            {
                if(elem->oper)
                {
                    if ((elem->oper == '/' || (elem->oper == '*' && (elem->right->oper =='+' || elem->right->oper=='-'
                        || elem->right->number < 0)))
                        || (elem->oper == '^' && elem->right->oper))
                    {
                        f << '(';
                        LRR (elem->right, f);
                        f << ')';
                    }
                    else
                    {
                        LRR (elem->right, f);
                    }
                }
                else
                {
                    LRR(elem->right, f);
                }
            }
        }
        else if (elem->func)
        {
            f << get_function_name (elem->func);
            f << "(";
            LRR (elem->left, f);
            f << ")";
        }
    }
}

void tree_elem::copy_elem (tree_elem *dest, tree_elem *src)
{
    if((src) && (dest))
    {
        dest->symbol = src->symbol;
        dest->oper = src->oper;
        dest->func = src->func;
        dest->number = src->number;
        dest->complex_oper = src->complex_oper;
        dest->right = src->right;
        dest->left = src->left;
    }
}

tree_elem* tree_elem::copy_tree(tree_elem* src)
{
    tree_elem* temp = nullptr;

    if (src)
    {
        temp = new tree_elem;
        temp->symbol = src->symbol;
        temp->oper = src->oper;
        temp->func = src->func;
        temp->number = src->number;
        temp->complex_oper = src->complex_oper;
        temp->right = copy_tree(src->right);
        temp->left = copy_tree(src->left);
    }

    return temp;
}

void tree_elem::Destroy_tree (tree_elem * elem)
{
    if(elem)
    {
        Destroy_tree (elem->left);
        Destroy_tree (elem->right);
        delete elem;
    }
}

int tree_elem::search_var (tree_elem * elem, char ch)
{
    if (elem)
    {
        return (elem->symbol == ch || search_var (elem->left, ch)
                || search_var (elem->right, ch));
    }

    else return 0;
}

bool tree_elem::isdgt (tree_elem * elem)
{
    if (elem && !elem->func && !elem->symbol && !elem->oper)
    {
        return true;
    }

    return false;
}

bool tree_elem::iszero (tree_elem * elem)
{
    if(elem && !elem->func && !elem->symbol && !elem->oper && !elem->number)
    {
        return true;
    }

    return false;
}

bool tree_elem::is_one(tree_elem *elem)
{
    if(elem && !elem->func && !elem->symbol && !elem->oper && elem->number == 1)
    {
        return true;
    }

    return false;
}

void tree_elem::ch_op (tree_elem * elem)
{
    if (elem)
    {
        if (elem->oper == '+')
        {
            elem->oper = '-';
        }
        else if (elem->oper == '-')
        {
            elem->oper = '+';
        }

        ch_op (elem->left);
        ch_op (elem->right);
    }
}

void tree_elem::change_oper (tree_elem * elem)
{
    if (elem)
    {
        if (elem->oper == '-')
        {
            if (elem->right->oper && elem->right->complex_oper)
            {
                ch_op (elem->right);
            }

            if (elem->right->oper == '+')
            {
                if (elem->right->left && elem->right->left->oper && elem->right->left->complex_oper)
                {
                    ch_op (elem->right->left);
                }
            }
        }

        change_oper (elem->left);
        change_oper (elem->right);
    }
}

void tree_elem::Calc(tree_elem * elem, char oper)
{
    if (elem)
    {
        if (elem->oper)
        {
            Calc (elem->left, oper);
            Calc (elem->right, elem->oper);

            if (isdgt (elem->left) && isdgt (elem->right))
            {
                if(elem->oper == '+' || elem->oper == '-')
                {
                    switch (elem->oper)
                    {
                        case '+':
                            elem->number = elem->left->number + elem->right->number;

                        case '-':
                            elem->number = elem->left->number + elem->right->number;
                    }

                    elem->oper = 0;
                    elem->complex_oper = 0;
                    delete elem->left;
                    delete elem->right;
                    elem->left = nullptr;
                    elem->right = nullptr;
                }
            }
        }
    }
}

void tree_elem::Simplify (tree_elem * elem)
{
    if (elem)
    {
        Simplify (elem->left);
        Simplify (elem->right);
        label:
        if (elem->oper)
        {
            if(iszero(elem->left) || iszero(elem->right))
            {
                if(elem->oper == '*' || elem->oper == '/' && iszero(elem->left))
                {
                    Destroy_tree(elem->left);
                    Destroy_tree(elem->right);
                    elem->left = nullptr;
                    elem->right = nullptr;
                    elem->oper = 0;
                    elem->number = 0;
                }
                else if(elem->oper == '+' || elem->oper == '-')
                {
                    if(iszero (elem->left))
                    {
                        if(elem->oper == '+')
                        {
                            Destroy_tree (elem->left);
                            tree_elem* temp = elem->right;
                            copy_elem (elem, elem->right);

                            if (temp)
                            {
                                delete temp;
                            }
                        }
                        else if (elem->oper=='-')
                        {
                            elem->oper = '*';
                            elem->left->number = -1;
                            goto label;
                        }
                    }
                    else if (iszero (elem->right))
                    {
                        Destroy_tree (elem->right);
                        tree_elem* temp = elem->left;
                        copy_elem (elem, elem->left);

                        if (temp)
                        {
                            delete temp;
                        }
                    }
                }
                else if (elem->oper == '^')
                {
                    Destroy_tree (elem->left);
                    elem->left = nullptr;
                    Destroy_tree (elem->right);
                    elem->right = nullptr;
                    elem->oper = 0;

                    if(iszero (elem->right))
                    {
                        elem->number = 1;
                    }
                    else
                    {
                        elem->number = 0;
                    }
                }
            }
            else if (isdgt(elem->left) && isdgt(elem->right) && elem->oper != '+' && elem->oper!='-')
            {
                switch (elem->oper)
                {
                    case '*':
                        elem->number = elem->left->number * elem->right->number;
                        break;
                    case '/':
                        elem->number = elem->left->number / elem->right->number;
                        break;
                    case '+':
                        elem->number = elem->left->number + elem->right->number;
                        break;
                    case '-':
                        elem->number = elem->left->number - elem->right->number;
                }

                elem->oper = 0;
                delete (elem->left);
                delete (elem->right);
                elem->left = nullptr;
                elem->right = nullptr;
            }

            if (elem->oper == '*')
            {
                if (is_one (elem->left))
                {
                    Destroy_tree (elem->left);
                    tree_elem* temp = elem->right;
                    copy_elem (elem, elem->right);

                    if (temp)
                    {
                        delete temp;
                    }
                }
                else if (is_one (elem->right))
                {
                    Destroy_tree (elem->right);
                    tree_elem* temp = elem->left;
                    copy_elem (elem, elem->left);

                    if (temp)
                    {
                        delete temp;
                    }
                }
            }

            if (elem->oper == '^' && is_one (elem->right))
            {
                delete elem->right;
                tree_elem* p = elem->left;
                copy_elem (elem, elem->left);
                delete p;
            }
        }
    }
}

void tree_elem::Diff (tree_elem* elem, char ch)
{
    if(elem)
    {
        if(search_var(elem, ch))
        {
            if(elem->oper)
            {
                switch(elem->oper)
                {
                    case '+':
                    case '-':
                        Diff (elem->left, ch);
                        Diff (elem->right, ch);
                        break;
                    case '*':
                    {
                        elem->oper = '+';
                        elem->complex_oper = 1;

                        auto temp1 = new tree_elem;
                        temp1->oper = '*';
                        temp1->left = copy_tree(elem->left);
                        Diff (temp1->left, ch);
                        temp1->right = copy_tree(elem->right);

                        auto *temp2 = new tree_elem;
                        temp2->oper = '*';
                        temp2->left = elem->left;
                        temp2->right = elem->right;
                        Diff(temp2->right, ch);

                        elem->left = temp1;
                        elem->right = temp2;
                        break;
                    }
                    case '/':
                    {
                        elem->oper = '/';

                        auto *temp1 = new tree_elem;
                        temp1->oper = '^';
                        temp1->left = copy_tree(elem->right);
                        temp1->right = new tree_elem;
                        temp1->right->number = 2;

                        auto *temp2 = new tree_elem;
                        temp2->oper = '*';
                        temp2->left = copy_tree(elem->left);
                        Diff(temp2->left, ch);
                        temp2->right = copy_tree(elem->right);

                        auto *temp3 = new tree_elem;
                        temp3->oper = '*';
                        temp3->left = elem->left;
                        temp3->right = elem->right;
                        Diff(temp3->right, ch);

                        auto *temp4 = new tree_elem;
                        temp4->oper = '-';
                        temp4->complex_oper = 1;
                        temp4->left = temp2;
                        temp4->right = temp3;

                        elem->left = temp4;
                        elem->right = temp1;
                        break;
                    }
                    case '^':
                    {
                        elem->oper = '*';
                        tree_elem *temp1;

                        if(isdgt(elem->right) && elem->right->number != 1)
                        {
                            temp1 = new tree_elem;
                            temp1->oper = '^';
                            temp1->left = elem->left;
                            temp1->right = elem->right;
                            elem->left = new tree_elem;
                            elem->left->number = elem->right->number;
                            temp1->right->number -= 1;
                            elem->right = temp1;
                        }
                        else
                        {
                            temp1 = new tree_elem;
                            temp1->oper = '^';
                            temp1->left = elem->left;
                            temp1->right = elem->right;
                            elem->left = temp1;

                            elem->right = new tree_elem;
                            elem->right->oper = '+';
                            elem->complex_oper = 1;

                            temp1 = new tree_elem;
                            temp1->oper = '*';
                            temp1->left = copy_tree(elem->left->right);
                            Diff(temp1->left, ch);
                            temp1->right = new tree_elem;
                            temp1->right->func = ln;
                            temp1->right->left = copy_tree(elem->left->left);

                            elem->right->left = temp1;

                            temp1 = new tree_elem;
                            temp1->oper = '*';
                            temp1->left = copy_tree(elem->left->right);
                            temp1->right = new tree_elem;
                            temp1->right->oper = '/';
                            temp1->right->left = copy_tree(elem->left->left);
                            Diff(temp1->right->left, ch);
                            temp1->right->right = copy_tree(elem->left->left);

                            elem->right->right = temp1;
                        }
                    }
                }
            }
            else if(elem->symbol)
            {
                elem->symbol = 0;
                elem->number = 1;
            }
            else if(elem->func)
            {
                auto temp1 = new tree_elem;
                copy_elem(temp1, elem);
                elem->func = notfunc;
                elem->oper = '*';
                elem->number = 1;

                elem->left = copy_tree(temp1->left);
                Diff(elem->left, ch);

                if (temp1->func == sin)
                {
                    elem->right = temp1;
                    elem->right->func = cos;
                }
                else if (temp1->func == cos)
                {
                    elem->right = new tree_elem;
                    elem->right->oper = '*';
                    elem->right->left = new tree_elem;
                    elem->right->left->number = -1;
                    elem->right->right = temp1;
                    elem->right->right->func = sin;
                }
                else if (temp1->func == arcsin)
                {
                    elem->right = new tree_elem;
                    elem->right->oper = '/';
                    elem->right->left = new tree_elem;

                    if(temp1->func == arccos)
                    {
                        elem->right->left->number = 1;
                    }

                    elem->right->right = new tree_elem;
                    elem->right->right->func = sqrt;

                    auto temp2 = new tree_elem;
                    temp2->oper = '-';
                    temp2->left = new tree_elem;
                    temp2->right = new tree_elem;
                    temp2->right->oper = '^';
                    temp2->right->left = temp1->left;
                    temp2->right->right = new tree_elem;
                    temp2->right->right->number = 2;

                    elem->right->right->left = temp2;
                }
                else if (temp1->func == arccos)
                {
                    elem->right = new tree_elem;
                    elem->right->oper = '/';
                    elem->right->left = new tree_elem;

                    if(temp1->func == arccos)
                    {
                        elem->right->left->number =-1;
                    }

                    elem->right->right = new tree_elem;
                    elem->right->right->func = sqrt;

                    auto temp2 = new tree_elem;
                    temp2->oper = '-';
                    temp2->left = new tree_elem;
                    temp2->right = new tree_elem;
                    temp2->right->oper = '^';
                    temp2->right->left = temp1->left;
                    temp2->right->right = new tree_elem;
                    temp2->right->right->number = 2;

                    elem->right->right->left = temp2;
                }
                else if (temp1->func == tg)
                {
                    elem->right = new tree_elem;
                    elem->right->oper = '+';
                    elem->right->left = new tree_elem;
                    elem->right->right = new tree_elem;
                    elem->right->right->oper = '^';
                    elem->right->right->left = temp1;
                    elem->right->right->right = new tree_elem;
                    elem->right->right->right->number = 2;
                }
                else if (temp1->func == arctg)
                {
                    elem->right = new tree_elem;
                    elem->right->oper = '/';
                    elem->right->left = new tree_elem;
                    elem->right->right = new tree_elem;
                    elem->right->right->oper = '+';
                    elem->right->right->left = new tree_elem;
                    elem->right->right->right = new tree_elem;
                    elem->right->right->right->oper = '^';
                    elem->right->right->right->left = temp1->left;
                    elem->right->right->right->right = new tree_elem;
                    elem->right->right->right->right->number = 2;
                }
                else if (temp1->func == sqrt)
                {
                    elem->right = new tree_elem;
                    elem->right->oper = '/';
                    elem->right->left = new tree_elem;
                    elem->right->right = new tree_elem;
                    elem->right->right->oper = '*';
                    elem->right->right->left = new tree_elem;
                    elem->right->right->left->number = 2;
                    elem->right->right->right = temp1;
                }
                else if (temp1->func == exp)
                {
                    elem->right = temp1;
                }
                else if (temp1->func == ln)
                {
                    elem->right = new tree_elem;
                    elem->right->oper = '/';
                    elem->right->left = new tree_elem;
                    elem->right->right = temp1->left;
                }
            }
        }
        else
        {
            elem->func = notfunc;
            elem->oper = 0;
            elem->symbol = 0;
            elem->number = 0;
            Destroy_tree(elem->left);
            Destroy_tree(elem->right);
            elem->left = nullptr;
            elem->right = nullptr;
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
    array[index] = 0;
}

void buffer::clear ()
{
    index = 0;
    array[index] = 0;
}

char* buffer::get_str (char* s)
{
    if(s)
    {
        strcpy(s, array);
    }
    return array;
}

char* get_token(char* str, int &ind)
{
    char * res = new char [15];

    if (isalpha (str[ind]) && isalpha (str[ind+1]))
    {
        buffer buf;

        while (isalpha (str[ind]))
        {
            buf.add (str[ind]);
            ind++;
        }
        strcpy (res,buf.get_str());
    }

    return res;
}

functions isfunc (char* str)
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
    return res;
}

char* get_function_name (functions index)
{
    char* res = nullptr;

    switch (index)
    {
        case 1: res = (char*) "sin"; break;
        case 2: res = (char*) "asin"; break;
        case 3: res = (char*) "cos"; break;
        case 4: res = (char*) "acos"; break;
        case 5: res = (char*) "tg"; break;
        case 6: res = (char*) "arctg"; break;
        case 7: res = (char*) "sqrt"; break;
        case 8: res = (char*) "exp"; break;
        case 9: res = (char*) "ln"; break;
        default: res = (char*) "Error";
    }

    return res;
}

int analysis(char *str, int &ind)
{
    int result = 1, bracket = 0;

    if (str[ind] == '(')
    {
        bracket = 1;
        ind++;
    }

    while (str[ind] && result && str[ind] != ')')
    {
        if (str[ind] == '(')
        {
            result = analysis (str,ind);
        }
        else
        {
            if (isalpha (str[ind]) && isalpha(str[ind + 1]))
            {
                buffer buf;

                while (isalpha(str[ind]))
                {
                    buf.add (str[ind]);
                    ind++;
                }

                ind--;
                result = (result && isfunc(buf.get_str()));
            }

            ind++;
        }
    }

    if (bracket)
    {
        if(str[ind] != ')')
        {
            result = 0;
        }

        ind++;
    }
    else if(str[ind] == ')')
    {
        result = 0;
    }

    return result;
}

int char_to_int (char ch)
{
    return ch - 48;
}

float get_number(char *str, int &cur)
{
    int mult1 = 10;
    float mult2 = 1;
    float res = 0;
    int sign = 1;

    if (str[cur] == '+' || str[cur]=='-')
    {
        if (str[cur] == '-')
        {
            sign -= 2;
        }

        cur++;
    }

    while(isdigit(str[cur]) || str[cur] == '.')
    {
        if(str[cur] == '.')
        {
            mult1 = 1;
            mult2 = 1;
        }
        else
        {
            res = res * mult1 + char_to_int (str[cur]) * mult2;
        }

        if(mult1 < 10)
        {
            mult2 *= 0.1;
        }

        cur++;
    }

    return res * sign;
}

void ignore_brackets(char * str, int &cur)
{
    if(str[cur] == '(')
    {
        while(str[cur] != ')')
        {
            cur++;

            if(str[cur] == '(')
            {
                ignore_brackets (str,cur);
            }
        }

        cur++;
    }
}



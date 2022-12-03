#ifndef DERIVATIVE_PARSER_H
#define DERIVATIVE_PARSER_H
#include <iostream>

enum functions
{
    notfunc, sin, arcsin, cos, arccos, tg, arctg, sqrt, exp, ln
};

class tree_elem
{
public:
    tree_elem *right, *left;
    char symbol;
    char oper;
    functions func;
    float number;
    char complex_oper;

    tree_elem ();

    void Processing (char*);
    tree_elem *build_tree (char*, int ind1 = 0, int ind2 = 0, int xxx = 0);
    tree_elem *mul_n_div (char*, int, int);
    tree_elem *create_leaf (char*, int, int);
    static bool isdgt (tree_elem*);
    static bool iszero (tree_elem*);
    static bool is_one (tree_elem*);
    void ch_op (tree_elem*);
    void change_oper (tree_elem*);
    void Calc (tree_elem*, char oper = '+');
    void Simplify (tree_elem *);
    void Diff (tree_elem*, char);
    void LRR (tree_elem*, std::ostream&);
    void copy_elem (tree_elem*, tree_elem*);
    tree_elem* copy_tree (tree_elem*);
    void Destroy_tree (tree_elem*);
    int search_var(tree_elem*, char);
};

class buffer
{
private:
    char array[15];
    int index;
public:

    buffer ();
    void add (char);
    void clear () ;
    char* get_str (char* s = nullptr);
};

char* get_token (char*, int&);
functions isfunc (char*);
char* get_function_name(functions);
int analysis(char*, int&);
int char_to_int (char);
float get_number (char*, int&);
void ignore_brackets (char*, int &);

#endif //DERIVATIVE_PARSER_H

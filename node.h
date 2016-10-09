//
// Created by NEzyaka on 28.09.16.
//

#ifndef TURNIP2_NODE_H
#define TURNIP2_NODE_H

#include <string>

class Node {
public:
    Node(int k) { kind = k; }

    enum node_type {
        VAR, CONST, ADD, SUB, MUL, DIV, LESS_TEST, MORE_TEST,
        LESS_IS_TEST, MORE_IS_TEST, IS_TEST, IS_NOT_TEST, SET, IF, ELSE, DO, WHILE,
        EMPTY, SEQ, EXPR, PROG, DEF, PRINT, INPUT, FUNCTION_DEFINE, FUNCTION_CALL, RETURN
    };

    int kind;
    struct Node *o1, *o2, *o3;

    enum val_type { null, integer, string };
    int value_type = val_type::null;

    int int_val = 0;
    std::string str_val = "";

    std::string var_name = "";
};


#endif //TURNIP2_NODE_H

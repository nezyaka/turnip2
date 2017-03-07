//
// Created by NEzyaka on 28.09.16.
//

#ifndef TURNIP2_NODE_H
#define TURNIP2_NODE_H

#include "type.h"
#include "location.h"
#include <memory>
#include <memory>
#include <string>

class Node {
public:
    explicit Node(unsigned short k = 255, std::shared_ptr<class Node> op1 = nullptr,
                  std::shared_ptr<class Node> op2 = nullptr, std::shared_ptr<class Node> op3 = nullptr)
            : kind(k), o1(op1), o2(op2), o3(op3) {}

    Location location;

    enum node_type {
        VAR_ACCESS, CONST, ARG, ARRAY_ACCESS, FUNCTION_CALL,
        OBJECT_CONSTRUCT,
        PROPERTY_ACCESS, METHOD_CALL,
        FUNC_OBJ_PROPERTY_ACCESS, FUNC_OBJ_METHOD_CALL,
        ARRAY, ARG_LIST,
        ADD, SUB, MUL, DIV,
        LESS_TEST, MORE_TEST,
        LESS_IS_TEST, MORE_IS_TEST,
        IS_TEST, IS_NOT_TEST,
        SET, RETURN,
        IF, ELSE,
        AND, OR, NOT,
        DO, WHILE, REPEAT,
        VAR_DEF, INIT, DELETE,
        EMPTY, SEQ, EXPR,
        PRINTLN, INPUT,
        FUNCTION_DEFINE, CLASS_DEFINE
    };

    unsigned short kind;
    std::shared_ptr<class Node> o1, o2, o3;

    enum val_type { VOID, INTEGER, FLOATING, STRING, BOOL, USER };
    enum access_type { PRIVATE, PUBLIC, PROTECTED };

    int value_type = val_type::VOID;
    std::string user_type = "";

    int int_val = -1;
    double float_val = 0.0;
    std::string str_val = "";

    std::map<std::string, std::pair<int, std::shared_ptr<Node>>> class_def_properties;
    std::map<std::string, std::shared_ptr<type>> func_def_args;
    std::vector<std::shared_ptr<Node>> func_call_args;

    std::string var_name = "";
    std::string property_name = "";
};


#endif //TURNIP2_NODE_H

//
// Created by NEzyaka on 29.09.16.
//

#include <iostream>
#include <llvm/ADT/APInt.h>
#include "generator.h"

void Generator::use_io() {
    io_using = true;

    printfArgs.push_back(Type::getInt8PtrTy(context));
    printfType = FunctionType::get(Type::getInt32Ty(context), printfArgs, true);
    printf = module->getOrInsertFunction("printf", printfType);

    scanfArgs.push_back(Type::getInt8PtrTy(context));
    scanfType = FunctionType::get(Type::getInt32Ty(context), scanfArgs, true);
    scanf = module->getOrInsertFunction("scanf", scanfType);
}

void Generator::generate(Node *n) {
    switch(n->kind) {
        case Node::DEF:
            table.insert(std::pair<std::string, Value *>(n->var_name, builder->CreateAlloca(Type::getInt32Ty(context),
                                                                                            nullptr,
                                                                                            n->var_name + "_ptr")));

            if (n->value_type == Node::integer) {
                generate(n->o1);
                Value *val = stack.top();
                stack.pop();

                builder->CreateStore(val, table.at(n->var_name));
            }

            break;
        case Node::VAR: {
            stack.push(builder->CreateLoad(Type::getInt32Ty(context), table.at(n->var_name), n->var_name));
            break;
        }
        case Node::CONST: {
            if (n->value_type == Node::string)
                stack.push(builder->CreateGlobalStringPtr(n->str_val));
            else if (n->value_type == Node::integer)
                stack.push(ConstantInt::get(context, APInt(32, static_cast<uint64_t>(n->int_val))));
            break;
        }
        case Node::ADD: {
            generate(n->o1);
            Value *left = stack.top();
            stack.pop();
            generate(n->o2);
            Value *right = stack.top();
            stack.pop();
            Value *sum = builder->CreateAdd(left, right, "add");
            stack.push(sum);

            break;
        }
        case Node::SUB: {
            generate(n->o1);
            Value *left = stack.top();
            stack.pop();
            generate(n->o2);
            Value *right = stack.top();
            stack.pop();
            Value *sum = builder->CreateSub(left, right, "sub");
            stack.push(sum);

            break;
        }
        case Node::MUL: {
            generate(n->o1);
            Value *left = stack.top();
            stack.pop();
            generate(n->o2);
            Value *right = stack.top();
            stack.pop();
            Value *sum = builder->CreateMul(left, right, "mul");
            stack.push(sum);

            break;
        }
        case Node::DIV: {
            generate(n->o1);
            Value *left = stack.top();
            stack.pop();
            generate(n->o2);
            Value *right = stack.top();
            stack.pop();
            Value *sum = builder->CreateSDiv(left, right, "div");
            stack.push(sum);

            break;
        }
        case Node::INCREMENT: {
            generate(n->o1);
            Value *left = stack.top();
            stack.pop();

            builder->CreateStore(builder->CreateAdd(left, ConstantInt::get(Type::getInt32Ty(context), APInt(32, 1))), table.at(left->getName()));

            break;
        }
        case Node::LESS_TEST: {
            generate(n->o1);
            Value *left = stack.top();
            stack.pop();

            generate(n->o2);
            Value *right = stack.top();
            stack.pop();

            stack.push(builder->CreateICmpSLT(left, right, "eq_cmp"));
            break;
        }
        case Node::LESS_IS_TEST: {
            generate(n->o1);
            Value *left = stack.top();
            stack.pop();

            generate(n->o2);
            Value *right = stack.top();
            stack.pop();

            stack.push(builder->CreateICmpSLE(left, right, "eq_cmp"));
            break;
        }
        case Node::MORE_TEST: {
            generate(n->o1);
            Value *left = stack.top();
            stack.pop();

            generate(n->o2);
            Value *right = stack.top();
            stack.pop();

            stack.push(builder->CreateICmpSGT(left, right, "sgt_cmp"));
            break;
        }
        case Node::MORE_IS_TEST: {
            generate(n->o1);
            Value *left = stack.top();
            stack.pop();

            generate(n->o2);
            Value *right = stack.top();
            stack.pop();

            stack.push(builder->CreateICmpSGE(left, right, "sge_cmp"));
            break;
        }
        case Node::IS_TEST: {
            generate(n->o1);
            Value *left = stack.top();
            stack.pop();

            generate(n->o2);
            Value *right = stack.top();
            stack.pop();

            stack.push(builder->CreateICmpEQ(left, right, "eq_cmp"));
            break;
        }
        case Node::IS_NOT_TEST: {
            generate(n->o1);
            Value *left = stack.top();
            stack.pop();

            generate(n->o2);
            Value *right = stack.top();
            stack.pop();

            stack.push(builder->CreateICmpNE(left, right, "ne_cmp"));
            break;
        }
        case Node::SET: {
            generate(n->o1);
            Value *val = stack.top();
            stack.pop();

            builder->CreateStore(val, table.at(n->var_name));

            break;
        }
        case Node::IF: {
            generate(n->o1);

            Function *parent = builder->GetInsertBlock()->getParent();

            BasicBlock *thenBlock = BasicBlock::Create(context, "then", parent);
            BasicBlock *elseBlock = BasicBlock::Create(context, "else");
            BasicBlock *mergeBlock = BasicBlock::Create(context, "ifcont");

            builder->CreateCondBr(stack.top(), thenBlock, elseBlock);

            builder->SetInsertPoint(thenBlock);
            generate(n->o2);

            builder->CreateBr(mergeBlock);

            parent->getBasicBlockList().push_back(mergeBlock);
            builder->SetInsertPoint(mergeBlock);

            break;
        }
        case Node::ELSE: {
            generate(n->o1);

            Function *parent = builder->GetInsertBlock()->getParent();

            BasicBlock *thenBlock = BasicBlock::Create(context, "then", parent);
            BasicBlock *elseBlock = BasicBlock::Create(context, "else");
            BasicBlock *mergeBlock = BasicBlock::Create(context, "ifcont");

            builder->CreateCondBr(stack.top(), thenBlock, elseBlock);

            builder->SetInsertPoint(thenBlock);
            generate(n->o2);

            builder->CreateBr(mergeBlock);

            parent->getBasicBlockList().push_back(elseBlock);
            builder->SetInsertPoint(elseBlock);
            generate(n->o3);

            builder->CreateBr(mergeBlock);

            parent->getBasicBlockList().push_back(mergeBlock);
            builder->SetInsertPoint(mergeBlock);

            break;
        }
        case Node::DO: {
            Function *parent = builder->GetInsertBlock()->getParent();
            BasicBlock *loopBlock = BasicBlock::Create(context, "loop", parent);
            builder->CreateBr(loopBlock);

            builder->SetInsertPoint(loopBlock);
            generate(n->o1);

            generate(n->o2);
            Value *condition = stack.top();
            stack.pop();

            BasicBlock *afterBlock = BasicBlock::Create(context, "afterloop", parent);
            builder->CreateCondBr(condition, loopBlock, afterBlock);
            builder->SetInsertPoint(afterBlock);

            break;
        }
        case Node::WHILE: {
            Function *parent = builder->GetInsertBlock()->getParent();
            BasicBlock *loopBlock = BasicBlock::Create(context, "loop", parent);
            builder->CreateBr(loopBlock);

            builder->SetInsertPoint(loopBlock);
            generate(n->o2);

            generate(n->o1);
            Value *condition = stack.top();
            stack.pop();

            BasicBlock *afterBlock = BasicBlock::Create(context, "afterloop", parent);
            builder->CreateCondBr(condition, loopBlock, afterBlock);
            builder->SetInsertPoint(afterBlock);

            break;
        }
        case Node::FUNCTION_DEFINE: {
            FunctionType *type = FunctionType::get(Type::getInt32Ty(context), false);
            Function *func = Function::Create(type, Function::ExternalLinkage, n->var_name, module);

            functions.insert(std::pair<std::string, Function*>(n->var_name, func));

            BasicBlock *entry = BasicBlock::Create(context, "entry", func);
            builder->SetInsertPoint(entry);
            currentBlock = entry;

            generate(n->o2);

            break;
        }
        case Node::SEQ:
            generate(n->o1);
            generate(n->o2);
            break;
        case Node::EXPR:
            generate(n->o1);
            break;
        case Node::RETURN: {
            generate(n->o1);

            builder->CreateRet(stack.top());
            stack.pop();
            stack.pop();

            break;
        }
        case Node::PRINT: {
            if (!io_using)
                use_io();

            generate(n->o1);
            std::vector<Value*> args;
            Value *format;

            if (stack.top()->getType() == Type::getInt32Ty(context))
                format = builder->CreateGlobalStringPtr("%d\n");
            else
                format = builder->CreateGlobalStringPtr("%s\n");

            args.push_back(format);
            args.push_back(stack.top());
            stack.pop();
            builder->CreateCall(printf, args);

            break;
        }
        case Node::INPUT: {
            if (!io_using)
                use_io();

            std::vector<Value*> args;
            Value *format = builder->CreateGlobalStringPtr("%d");

            args.push_back(format);
            args.push_back(table.at(n->var_name));
            builder->CreateCall(scanf, args);

            break;
        }
        case Node::PROG:
            generate(n->o1);
            break;
    }
}

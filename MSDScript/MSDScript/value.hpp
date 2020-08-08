//
//  value.hpp
//  HW3Testing
//
//  Created by Jonathan Sullivan on 1/30/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//

#ifndef value_hpp
#define value_hpp

#include <stdio.h>
#include <string>

#include "environment.hpp"

/* A forward declaration, so `Val` can refer to `Expr`, while
   `Expr` still needs to refer to `Val`. */
class Expr;
class Cont;

class Value ENABLE_THIS(Value)
{
public:
    virtual bool equals(PTR(Value) val) = 0;
    virtual PTR(Value) add_to(PTR(Value) otherValue) = 0;
    virtual PTR(Value) mult_with(PTR(Value) otherValue) = 0;
    virtual PTR(Expr) to_expr() = 0;
    virtual std::string to_string() = 0;
    virtual PTR(Value) call (PTR(Value) actualArg) = 0;
    virtual bool is_true() = 0;
    virtual void callStep(PTR(Value) actualArgVal, PTR(Cont) rest) = 0;
};

class NumVal : public Value
{
public:
    int representation;
    NumVal(int rep);
    bool equals(PTR(Value) val);
    PTR(Value) add_to(PTR(Value) otherValue);
    PTR(Value) mult_with(PTR(Value) otherValue);
    PTR(Expr) to_expr();
    std::string to_string();
    virtual PTR(Value) call (PTR(Value) actualArg);
    bool is_true();
    void callStep(PTR(Value) actualArgVal, PTR(Cont) rest);
};

class BoolVal : public Value
{
public:
    bool representation;
    BoolVal(bool rep);
    bool equals(PTR(Value) val);
    PTR(Value) add_to(PTR(Value) otherValue);
    PTR(Value) mult_with(PTR(Value) otherValue);
    PTR(Expr) to_expr();
    std::string to_string();
    virtual PTR(Value) call (PTR(Value) actualArg);
    bool is_true();
    void callStep(PTR(Value) actualArgVal, PTR(Cont) rest);
};

class FunVal : public Value
{
public:
    std::string formalArg;
    PTR(Expr) body;
    PTR(Env) env;
    FunVal(std::string formalArg, PTR(Expr) body, PTR(Env) env);
    bool equals(PTR(Value) val);
    PTR(Value) add_to(PTR(Value) otherValue);
    PTR(Value) mult_with(PTR(Value) otherValue);
    PTR(Expr) to_expr();
    std::string to_string();
    virtual PTR(Value) call (PTR(Value) actualArg);
    bool is_true();
    void callStep(PTR(Value) actualArgVal, PTR(Cont) rest);
};

#endif /* value_hpp */

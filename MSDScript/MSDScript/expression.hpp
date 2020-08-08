//
//  expression.hpp
//  HW1ArithmeticParser
//
//  Created by Jonathan Sullivan on 1/11/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//

#ifndef expression_hpp
#define expression_hpp

#include <stdio.h>
#include <string>
#include "value.hpp"

class Expr ENABLE_THIS(Expr){
public:
    virtual bool equals(PTR(Expr) e) = 0;
    
    virtual PTR(Value) interp(PTR(Env) env) = 0;
    
    virtual void step_interp() = 0;
    
    virtual PTR(Expr) optimize() = 0;

    virtual PTR(Expr) substitute(std::string variable, PTR(Expr) newValue) = 0;
    
    virtual bool canInterp() = 0;
    
    virtual std::string to_string() = 0;
};

class NumExpr : public Expr {
public:
    int representation;
    PTR(Value) numVal;  

    NumExpr(int value);

    bool equals(PTR(Expr) e);
    
    PTR(Value) interp(PTR(Env) env);
    
    void step_interp();
    
    PTR(Expr) optimize();
    
    PTR(Expr) substitute(std::string variable, PTR(Expr) newValue);
    
    bool canInterp();
    
    std::string to_string();
};

class VarExpr : public Expr {
public:
    std::string name;
    
    VarExpr(std::string name);
    
    bool equals(PTR(Expr) e);
    
    PTR(Value) interp(PTR(Env) env);
    
    void step_interp();
    
    PTR(Expr) optimize();
    
    PTR(Expr) substitute(std::string variable, PTR(Expr) newValue);
    
    bool canInterp();
    
    std::string to_string();
};

class BoolExpr: public Expr{
public:
    bool representation;
    
    BoolExpr(bool rep);
    
    bool equals(PTR(Expr) e);
    
    PTR(Value) interp(PTR(Env) env);
    
    void step_interp();
    
    PTR(Expr) optimize();
    
    PTR(Expr) substitute(std::string var, PTR(Expr) val);
    
    bool canInterp();
    
    std::string to_string();
};

class AddExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;

    AddExpr(PTR(Expr) lhs, PTR(Expr) rhs);

    bool equals(PTR(Expr) e);
    
    PTR(Value) interp(PTR(Env) env);
    
    void step_interp();
    
    PTR(Expr) optimize();
    
    PTR(Expr) substitute(std::string variable, PTR(Expr) newValue);
    
    bool canInterp();
    
    std::string to_string();
};

class MultExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;

    MultExpr(PTR(Expr) lhs, PTR(Expr) rhs);

    bool equals(PTR(Expr) e);
    
    PTR(Value) interp(PTR(Env) env);
    
    void step_interp();
    
    PTR(Expr) optimize();
    
    PTR(Expr) substitute(std::string variable, PTR(Expr) newValue);
    
    bool canInterp();
    
    std::string to_string();
};

class LetExpr : public Expr {
public:  
    std::string variableName;
    PTR(Expr) rhs;
    PTR(Expr) body;
    
    LetExpr(std::string variableName, PTR(Expr) rhs, PTR(Expr) body);
    
    bool equals(PTR(Expr) e);
    
    PTR(Value) interp(PTR(Env) env);
    
    PTR(Expr) optimize();
    
    PTR(Expr) interpExpr();
    
    void step_interp();
    
    PTR(Expr) substitute(std::string variable, PTR(Expr) newValue);
    
    bool canInterp();
    
    std::string to_string();
};

class IfExpr: public Expr{
public:
    PTR(Expr) testExpr;
    PTR(Expr) thenExpr;
    PTR(Expr) elseExpr;
    
    IfExpr(PTR(Expr) testExpr, PTR(Expr) thenExpr, PTR(Expr) elseExpr);
    
    bool equals(PTR(Expr) e);
    
    PTR(Value) interp(PTR(Env) env);
    
    void step_interp();
    
    PTR(Expr) optimize();
    
    PTR(Expr) substitute(std::string var, PTR(Expr) val);
    
    bool canInterp();
    
    std::string to_string();
};

class EqualExpr: public Expr{
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;
    
    EqualExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    
    bool equals(PTR(Expr) e);
    
    PTR(Value) interp(PTR(Env) env);
    
    void step_interp();
    
    PTR(Expr) optimize();
    
    PTR(Expr) substitute(std::string var, PTR(Expr) val);
    
    bool canInterp();
    
    std::string to_string();
};

class FunExpr: public Expr{
public:
    std::string formalArgument;
    PTR(Expr) body;

    FunExpr(std::string formalArgument, PTR(Expr) body);

    bool equals(PTR(Expr) e);

    PTR(Value) interp(PTR(Env) env);
    
    void step_interp();

    PTR(Expr) optimize();

    PTR(Expr) substitute(std::string var, PTR(Expr) val);
    
    bool canInterp();
    
    std::string to_string();
};

class CallExpr: public Expr{
public:
    PTR(Expr) toBeCalled;
    PTR(Expr) actualArg;
    
    CallExpr(PTR(Expr) toBeCalled, PTR(Expr) actualArg);
    
    bool equals(PTR(Expr) e);
    
    PTR(Value) interp(PTR(Env) env);
    
    void step_interp();  
    
    PTR(Expr) optimize();
    
    PTR(Expr) substitute(std::string var, PTR(Expr) val);
    
    bool canInterp();
    
    std::string to_string();
};  

#endif /* expression_hpp */

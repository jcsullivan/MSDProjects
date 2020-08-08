//
//  cont.hpp
//  HW3Testing
//
//  Created by Jonathan Sullivan on 3/24/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//

#ifndef cont_hpp
#define cont_hpp

#include <stdio.h>
#include <string>
#include "pointer.hpp" 

class Expr;
class Value;
class Env;  

class Cont ENABLE_THIS(Cont)
{
public:
    virtual void step_continue() = 0;
    
    static PTR(Cont) done;
};

class DoneCont : public Cont {
public:
    DoneCont();
    void step_continue();
};

class RightThenAddCont : public Cont {
public:
    PTR(Expr) rhs;
    PTR(Env) env;
    PTR(Cont) rest;
    
    RightThenAddCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class AddCont : public Cont {
public:
    PTR(Value) lhs_val;
    PTR(Cont) rest;
    
    AddCont(PTR(Value) lhs_val, PTR(Cont) rest);
    void step_continue();
};

class RightThenMultCont : public Cont
{
public:
    PTR(Expr) rhs;
    PTR(Env) env;
    PTR(Cont) rest;
    
    RightThenMultCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class MultCont : public Cont
{
public:
    PTR(Value) lhs_val;
    PTR(Cont) rest;
    
    MultCont(PTR(Value) lhs_val, PTR(Cont) rest);
    void step_continue();
};

class RightThenEqualCont : public Cont
{
public:
    PTR(Expr) rhs;
    PTR(Env) env;
    PTR(Cont) rest;
    
    RightThenEqualCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class EqualCont : public Cont
{
public:
    PTR(Value) lhs_val;
    PTR(Cont) rest;
    
    EqualCont(PTR(Value) lhs_val, PTR(Cont) rest);
    void step_continue();
};

class IfBranchCont : public Cont
{
public:
    PTR(Expr) then_part;
    PTR(Expr) else_part;
    PTR(Env) env;
    PTR(Cont) rest;
    
    IfBranchCont(PTR(Expr) then_part, PTR(Expr) else_part, PTR(Env) env, PTR(Cont) rest);
    void step_continue();  
};

class ArgThenCallCont : public Cont
{
public:
    PTR(Expr) actualArg;
    PTR(Env) env;
    PTR(Cont) rest;
    
    ArgThenCallCont(PTR(Expr) actualArg, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class CallCont : public Cont
{
public:
    PTR(Value) toBeCalledVal;
    PTR(Cont) rest;
    
    CallCont(PTR(Value) toBeCalledVal, PTR(Cont) rest);
    void step_continue();  
};

class LetBodyCont : public Cont
{
public:  
    std::string var;
    PTR(Expr) body;
    PTR(Env) env;
    PTR(Cont) rest;
    
    LetBodyCont(std::string var, PTR(Expr) body, PTR(Env) env, PTR(Cont) rest);
    void step_continue();  
};  

#endif /* cont_hpp */

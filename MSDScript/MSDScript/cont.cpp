//
//  cont.cpp
//  HW3Testing
//
//  Created by Jonathan Sullivan on 3/24/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//

#include <stdexcept>
#include "cont.hpp"
#include "step.hpp"
#include "value.hpp"

PTR(Cont) Cont::done = NEW(DoneCont)();

DoneCont::DoneCont() { }

void DoneCont::step_continue() {
    throw std::runtime_error("can't continue done");
}

RightThenAddCont::RightThenAddCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest)
{ 
    this->rhs = rhs;
    this->env = env;
    this->rest = rest;
}

void RightThenAddCont::step_continue()
{
    PTR(Value) lhs_val = Step::val;
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = env;
    Step::cont = NEW(AddCont)(lhs_val, rest);
}

AddCont::AddCont(PTR(Value) lhs_val, PTR(Cont) rest) {
    this->lhs_val = lhs_val;
    this->rest = rest;
}

void AddCont::step_continue() {
    PTR(Value) rhs_val = Step::val;
    Step::mode = Step::continue_mode;
    Step::val = lhs_val->add_to(rhs_val);
    Step::cont = rest;
}

RightThenMultCont::RightThenMultCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest)
{
    this->rhs = rhs;
    this->env = env;
    this->rest = rest;
}

void RightThenMultCont::step_continue()
{
    PTR(Value) lhs_val = Step::val;
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = env;
    Step::cont = NEW(MultCont)(lhs_val, rest);
}

MultCont::MultCont(PTR(Value) lhs_val, PTR(Cont) rest)
{
    this->lhs_val = lhs_val;
    this->rest = rest;
}

void MultCont::step_continue()
{
    PTR(Value) rhs_val = Step::val;
    Step::mode = Step::continue_mode;
    Step::val = lhs_val->mult_with(rhs_val);
    Step::cont = rest;  
}

RightThenEqualCont::RightThenEqualCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest)
{
    this->rhs = rhs;
    this->env = env;
    this->rest = rest;
}

void RightThenEqualCont::step_continue()
{
    PTR(Value) lhs_val = Step::val;
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = env;
    Step::cont = NEW(EqualCont)(lhs_val, rest);
}

EqualCont::EqualCont(PTR(Value) lhs_val, PTR(Cont) rest)
{
    this->lhs_val = lhs_val;
    this->rest = rest;
}

void EqualCont::step_continue()
{
    PTR(Value) rhs_val = Step::val;
    Step::mode = Step::continue_mode;
    Step::val = NEW(BoolVal)(lhs_val->equals(rhs_val));
    Step::cont = rest;  
}

IfBranchCont::IfBranchCont(PTR(Expr)then_part, PTR(Expr)else_part, PTR(Env) env, PTR(Cont) rest)
{
    this->then_part = then_part;
    this->else_part = else_part;
    this->env = env;
    this->rest = rest;
}

void IfBranchCont::step_continue()
{
    PTR(Value) test_val = Step::val;
    Step::mode = Step::interp_mode;
    if (test_val->is_true())
    {
        Step::expr = then_part;
    }
    else
    {
        Step::expr = else_part;
    }
    Step::env = env;
    Step::cont = rest;  
}

ArgThenCallCont::ArgThenCallCont(PTR(Expr) actualArg, PTR(Env) env, PTR(Cont) rest)
{
    this->actualArg = actualArg;
    this->env = env;
    this->rest = rest;
}

void ArgThenCallCont::step_continue()
{
    Step::mode = Step::interp_mode;
    Step::expr = actualArg;
    Step::env = env;
    Step::cont = NEW(CallCont)(Step::val, rest);  
}

CallCont::CallCont(PTR(Value) toBeCalledVal, PTR(Cont) rest)
{
    this->toBeCalledVal = toBeCalledVal;
    this->rest = rest;
}

void CallCont::step_continue()
{
    toBeCalledVal->callStep(Step::val, rest);
}

LetBodyCont::LetBodyCont(std::string var, PTR(Expr) body, PTR(Env) env, PTR(Cont) rest)
{
    this->var = var;
    this->body = body;
    this->env = env;
    this->rest = rest;
}

void LetBodyCont::step_continue()
{
    Step::mode = Step::interp_mode;
    Step::expr = body;
    Step::env = NEW(ExtendedEnv)(var, Step::val, env);
    Step::cont = rest;  
}

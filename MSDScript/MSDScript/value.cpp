//
//  value.cpp
//  HW3Testing
//
//  Created by Jonathan Sullivan on 1/30/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//

#include "value.hpp"
#include "expression.hpp"
#include "step.hpp"
#include "cont.hpp"

NumVal::NumVal(int rep)
{
    this->representation = rep;
}

bool NumVal::equals(PTR(Value) otherValue)
{
    PTR(NumVal) otherNumValue = CAST(NumVal)(otherValue);
    if(otherNumValue == nullptr)
    {
        throw std::runtime_error("Right side of equality is not a number.");
    }
    else
    {
        return representation == otherNumValue->representation;
    }
}

PTR(Value) NumVal::add_to(PTR(Value) otherValue)
{
    PTR(NumVal) otherNumValue = CAST(NumVal)(otherValue);
    if(otherNumValue == nullptr)
    {
        throw std::runtime_error("Right side of add is not a number.");
    }
    else
    {
        return NEW(NumVal)((unsigned)representation + (unsigned)(otherNumValue->representation));
    }
}

PTR(Value) NumVal::mult_with(PTR(Value) otherValue)
{
    PTR(NumVal) otherNumValue = CAST(NumVal)(otherValue);
    if(otherNumValue == nullptr)
    {
        throw std::runtime_error("Right side of mult is not a number.");
    }
    else
    {
        return NEW(NumVal)((unsigned)representation * (unsigned)(otherNumValue->representation));
    }
}

PTR(Expr) NumVal::to_expr()
{
    return NEW(NumExpr)(representation);
}

std::string NumVal::to_string()
{
    return std::to_string(representation);
}

PTR(Value) NumVal::call(PTR(Value) actualArg)
{
    return THIS;
}

bool NumVal::is_true()
{
    return false;
}

void NumVal::callStep(PTR(Value) actualArgVal, PTR(Cont) rest)
{
    throw std::runtime_error("No call step for nums.");
}

TEST_CASE( "NumVal coverage." )
{
    CHECK ( (NEW(NumVal)(2))->call(NEW(BoolVal)(true))->equals(NEW(NumVal)(2)));
}

BoolVal::BoolVal(bool rep)
{
    this->representation = rep;
}

bool BoolVal::equals(PTR(Value) otherValue)
{
    PTR(BoolVal) otherBoolVal = CAST(BoolVal)(otherValue);
    if (otherBoolVal == nullptr)
    {
        return false;
    }
    else
    {
        return representation == otherBoolVal->representation;
    }
}

PTR(Value) BoolVal::add_to(PTR(Value) otherValue)
{
    throw std::runtime_error("No adding booleans.");
}

PTR(Value) BoolVal::mult_with(PTR(Value) otherValue)
{
    throw std::runtime_error("No multiplying booleans.");
}

PTR(Expr) BoolVal::to_expr()
{
    return NEW(BoolExpr)(representation);
}

std::string BoolVal::to_string()
{
    if(representation)
    {
        return "_true";
    }
    else
    {
        return "_false";
    }
}

PTR(Value) BoolVal::call(PTR(Value) actualArg)
{
    return THIS;
}

bool BoolVal::is_true()
{
    return representation;
}

void BoolVal::callStep(PTR(Value) actualArgVal, PTR(Cont) rest)
{
    throw std::runtime_error("No call step for bools.");
}

TEST_CASE( "BoolVal coverage." )
{
    CHECK ( (NEW(BoolVal)(true))->call(NEW(NumVal)(3))->equals(NEW(BoolVal)(true)));
}

FunVal::FunVal(std::string formalArg, PTR(Expr) body, PTR(Env) env)
{
    this->formalArg = formalArg;
    this->body = body;
    this->env = env;
}

bool FunVal::equals(PTR(Value) otherValue)
{
    PTR(FunVal) otherFunVal = CAST(FunVal)(otherValue);
    if (otherFunVal == nullptr)
    {
        return false;
    }
    else
    {
        return (formalArg == otherFunVal->formalArg) && (body == otherFunVal->body);
    }
}

PTR(Value) FunVal::add_to(PTR(Value) otherValue)
{
    throw std::runtime_error("No adding functions.");
}

PTR(Value) FunVal::mult_with(PTR(Value) otherValue)
{
    throw std::runtime_error("No multiplying functions.");
}

PTR(Expr) FunVal::to_expr()
{
    return NEW(FunExpr)(formalArg, body);
}

std::string FunVal::to_string()
{
//    return "(" + formalArg + ")(" + body->stringify() + ")";
    return "[function]";  
}

PTR(Value) FunVal::call(PTR(Value) actualArg)
{
    return this->body->interp(NEW(ExtendedEnv)(formalArg, actualArg, env));
//    return this->body->optimize()->substitute(formalArg, actualArg->to_expr())->optimize()->interp();
}

bool FunVal::is_true()
{
    return false;  
}

void FunVal::callStep(PTR(Value) actualArgVal, PTR(Cont) rest)
{
    Step::mode = Step::interp_mode;
    Step::expr = body;
    Step::env = NEW(ExtendedEnv)(formalArg, actualArgVal, env);
    Step::cont = rest;  
}

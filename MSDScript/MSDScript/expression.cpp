//
//  expression.cpp
//  HW1ArithmeticParser
//
//  Created by Jonathan Sullivan on 1/11/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//

#include "expression.hpp"
#include "cont.hpp"
#include "step.hpp"

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

NumExpr::NumExpr(int value)
{
    this->representation = value;
    this->numVal = NEW(NumVal)(value);
}

bool NumExpr::equals(PTR(Expr) e)
{
    PTR(NumExpr) n = CAST(NumExpr)(e);
    if (n == NULL)
    {
        return false;
    }
    else
    {
        return representation == n->representation;
    }
}

PTR(Value) NumExpr::interp(PTR(Env) env)
{
    return this->numVal;
}

void NumExpr::step_interp()
{
    Step::mode = Step::continue_mode;
    Step::val = NEW(NumVal)(representation);
    Step::cont = Step::cont;
}

PTR(Expr) NumExpr::optimize()
{
    return NEW(NumExpr)(representation);
}

PTR(Expr) NumExpr::substitute(std::string variable, PTR(Expr) newValue)
{
    return NEW(NumExpr)(representation);
}

bool NumExpr::canInterp()
{
    return true;
}

std::string NumExpr::to_string()
{
    return std::to_string(representation);
}

VarExpr::VarExpr(std::string name)
{
    this->name = name;
}

bool VarExpr::equals(PTR(Expr) e)
{
    PTR(VarExpr) v = CAST(VarExpr)(e);
    if (v == NULL)
    {
        return false;
    }
    else
    {
        return name == v->name;
    }
}

PTR(Value) VarExpr::interp(PTR(Env) env)
{
    return env->lookup(this->name);  
}

void VarExpr::step_interp()
{
    Step::mode = Step::continue_mode;
    Step::val = Step::env->lookup(this->name);
    Step::cont = Step::cont;
}

PTR(Expr) VarExpr::optimize()
{
    return NEW(VarExpr)(name);
}

PTR(Expr) VarExpr::substitute(std::string variable, PTR(Expr) newValue)
{
    if (name == variable)
    {
        return newValue;
    }
    else
    {
        return NEW(VarExpr)(name);
    }
}

bool VarExpr::canInterp()
{
    return false;
}

std::string VarExpr::to_string()
{
    return name;
}

BoolExpr::BoolExpr(bool rep)
{
    this->representation = rep;
}

bool BoolExpr::equals(PTR(Expr) e)
{
    PTR(BoolExpr) b = CAST(BoolExpr)(e);
    if (b == NULL)
    {
        return false;
    }
    else
    {
        return representation == b->representation;
    }
}

TEST_CASE( "Boolean coverage." )
{
    CHECK ( (NEW(BoolExpr)(true))->equals(NEW(BoolExpr)(true)));
    CHECK ( !(NEW(BoolExpr)(true))->equals(NEW(NumExpr)(2)));
}

PTR(Value) BoolExpr::interp(PTR(Env) env)
{
    return NEW(BoolVal)(representation);
}

void BoolExpr::step_interp()
{
    Step::mode = Step::continue_mode;
    Step::val = NEW(BoolVal)(representation);
    Step::cont = Step::cont;
}

PTR(Expr) BoolExpr::optimize()
{
    return NEW(BoolExpr)(representation);
}

PTR(Expr) BoolExpr::substitute(std::string var, PTR(Expr) val)
{
    return NEW(BoolExpr)(representation);
}

bool BoolExpr::canInterp()
{
    return true;
}

std::string BoolExpr::to_string()
{
    if (representation)
    {
        return "_true";
    }
    else
    {
        return "_false";
    }
}

AddExpr::AddExpr(PTR(Expr) lhs, PTR(Expr) rhs)
{
    this->lhs = lhs;
    this->rhs = rhs;
}

bool AddExpr::equals(PTR(Expr) e)
{
    PTR(AddExpr) a = CAST(AddExpr)(e);
    if (a == NULL)
    {
        return false;
    }
    else
    {
        return (lhs->equals(a->lhs) && rhs->equals(a->rhs));
    }
}

TEST_CASE( "Add coverage." )
{
    CHECK ( !(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))->equals(NEW(BoolExpr)(true)));
}

PTR(Value) AddExpr::interp(PTR(Env) env)
{
    return lhs->interp(env)->add_to(rhs->interp(env));
}

void AddExpr::step_interp()
{
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env;
    Step::cont = NEW(RightThenAddCont)(rhs, Step::env, Step::cont);
}

PTR(Expr) AddExpr::optimize()
{
    if (!THIS->canInterp())
    {
        return NEW(AddExpr)(lhs->optimize(), rhs->optimize());
    }
    else
    {
        PTR(EmptyEnv) env = NEW(EmptyEnv)();
        PTR(Expr) returnNum = THIS->interp(env)->to_expr();
        return returnNum;
    }
}

PTR(Expr) AddExpr::substitute(std::string variable, PTR(Expr) newValue)
{
    PTR(Expr) returnVal = NEW(AddExpr)(lhs->substitute(variable, newValue), rhs->substitute(variable, newValue));
    return returnVal;
}

bool AddExpr::canInterp()
{
    return (lhs->canInterp() && rhs->canInterp());
}

std::string AddExpr::to_string()
{
    if (!THIS->canInterp())
    {
        std::string addString = "( " + lhs->to_string() + " + " + rhs->to_string() + " )";
        return addString;
    }
    else
    {
        PTR(EmptyEnv) env = NEW(EmptyEnv)();
        PTR(Value) addedValue = THIS->interp(env);
        return addedValue->to_string();
    }
}

MultExpr::MultExpr(PTR(Expr) lhs, PTR(Expr) rhs)
{
    this->lhs = lhs;
    this->rhs = rhs;
}

bool MultExpr::equals(PTR(Expr) e)
{
    PTR(MultExpr) m = CAST(MultExpr)(e);
    if (m == NULL)
    {
        return false;
    }
    else
    {
        return (lhs->equals(m->lhs) && rhs->equals(m->rhs));
    }
}

TEST_CASE( "Mult coverage." )
{
    CHECK ( !(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))->equals(NEW(BoolExpr)(true)));
}

PTR(Value) MultExpr::interp(PTR(Env) env)
{
    return lhs->interp(env)->mult_with(rhs->interp(env));
}

void MultExpr::step_interp()
{
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env;
    Step::cont = NEW(RightThenMultCont)(rhs, Step::env, Step::cont);
}

PTR(Expr) MultExpr::optimize()
{
    if (!THIS->canInterp())
    {
        return NEW(MultExpr)(lhs->optimize(), rhs->optimize());
    }
    else
    {
        PTR(EmptyEnv) env = NEW(EmptyEnv)();
        PTR(Expr) returnMult = THIS->interp(env)->to_expr();
        return returnMult;
    }
}

PTR(Expr) MultExpr::substitute(std::string variable, PTR(Expr) newValue)
{
    PTR(Expr) returnVal = NEW(MultExpr)(lhs->substitute(variable, newValue), rhs->substitute(variable, newValue));
    return returnVal;
}

std::string MultExpr::to_string()
{
    if (!THIS->canInterp())
    {
        std::string multString = "( " + lhs->to_string() + " * " + rhs->to_string() + " )";
        return multString;
    }
    else
    {
        PTR(EmptyEnv) env = NEW(EmptyEnv)();
        PTR(Value) multipliedValue = THIS->interp(env);
        return multipliedValue->to_string();
    }
}

bool MultExpr::canInterp()
{
    return (lhs->canInterp() && rhs->canInterp());
}

LetExpr::LetExpr(std::string variableName, PTR(Expr) rhs, PTR(Expr) body)
{
    this->variableName = variableName;
    this->rhs = rhs;
    this->body = body;
}

bool LetExpr::equals(PTR(Expr) e)
{
    PTR(LetExpr) le = CAST(LetExpr)(e);
    if (le == nullptr)
    {
        return false;
    }
    else
    {
        return (this->variableName == variableName && this->rhs->equals(rhs) && this->body->equals(body));
    }
}

TEST_CASE( "Let coverage 1." )
{
    CHECK ( !(NEW(LetExpr)("x", NEW(NumExpr)(2), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2))))->equals(NEW(BoolExpr)(true)));
    CHECK ( (NEW(LetExpr)("x", NEW(NumExpr)(2), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2))))->equals(NEW(LetExpr)("x", NEW(NumExpr)(2), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2)))));
}

PTR(Value) LetExpr::interp(PTR(Env) env)
{
    PTR(Value) rhsValue = rhs->interp(env);
    PTR(Env) newEnv = NEW(ExtendedEnv)(variableName, rhsValue, env);
    return body->interp(newEnv);
//    return body->substitute(variableName, rhs)->interp();
}

void LetExpr::step_interp()
{
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = Step::env;
    Step::cont = NEW(LetBodyCont)(variableName, body, Step::env, Step::cont);
}

PTR(Expr) LetExpr::optimize()
{
    return body->optimize()->substitute(variableName, rhs->optimize());
}

PTR(Expr) LetExpr::substitute(std::string variable, PTR(Expr) newValue)
{
    if(variable == this->variableName)
    {
        PTR(LetExpr) returnData = NEW(LetExpr)(variableName, rhs->substitute(variableName, newValue), body);
        return returnData;
    }
    else
    {
        PTR(LetExpr) returnData = NEW(LetExpr)(variableName, rhs->substitute(variable, newValue), body->substitute(variable, newValue));
        return returnData;
    }
//    return body->substitute(variable, newValue);
}

bool LetExpr::canInterp()
{
    return true;
}

TEST_CASE( "Let coverage 2." )
{
    CHECK ( (NEW(LetExpr)("x", NEW(NumExpr)(2), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2))))->canInterp());
}

std::string LetExpr::to_string()
{
    PTR(EmptyEnv) env = NEW(EmptyEnv)();
    std::string returnString = THIS->interp(env)->to_string();
    return returnString;
}

IfExpr::IfExpr(PTR(Expr) testExpr, PTR(Expr) thenExpr, PTR(Expr) elseExpr)
{
    this->testExpr = testExpr;
    this->thenExpr = thenExpr;
    this->elseExpr = elseExpr;
}

bool IfExpr::equals(PTR(Expr) e)
{
    PTR(IfExpr) i = CAST(IfExpr)(e);
    if (i == NULL)
    {
        return false;
    }
    else
    {
        return (testExpr->equals(i->testExpr) && thenExpr->equals(i->thenExpr) && elseExpr->equals(i->elseExpr));
    }
}

TEST_CASE( "If coverage 1." )
{
    CHECK ( !(NEW(IfExpr)(NEW(BoolExpr)(true), NEW(NumExpr)(2), NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2))))->equals(NEW(BoolExpr)(true)));
    CHECK ( (NEW(IfExpr)(NEW(BoolExpr)(true), NEW(NumExpr)(2), NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2))))->equals(NEW(IfExpr)(NEW(BoolExpr)(true), NEW(NumExpr)(2), NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))));
}

PTR(Value) IfExpr::interp(PTR(Env) env)
{
    if (testExpr->interp(env)->equals(NEW(BoolVal)(true)))
    {
        return thenExpr->interp(env);
    }
    else
    {
        return elseExpr->interp(env);
    }
}

void IfExpr::step_interp()
{
    Step::mode = Step::interp_mode;
    Step::expr = testExpr;
    Step::env = Step::env;
    Step::cont = NEW(IfBranchCont)(thenExpr, elseExpr, Step::env, Step::cont);
}

PTR(Expr) IfExpr::optimize()
{
    if (!THIS -> canInterp())
    {
        PTR(Expr) tempTestExpr = this->testExpr;
        PTR(Expr) tempThenExpr = this->thenExpr;
        PTR(Expr) tempElseExpr = this->elseExpr;
        if (tempTestExpr->canInterp())
        {
            tempTestExpr = tempTestExpr->optimize();
        }
        if (tempThenExpr->canInterp())
        {
            tempThenExpr = tempThenExpr->optimize();
        }
        if (tempElseExpr->canInterp())
        {
            tempElseExpr = tempElseExpr->optimize();
        }
        PTR(Expr) returnData = NEW(IfExpr)(tempTestExpr, tempThenExpr, tempElseExpr);
        return returnData;
    }
    else
    {
        PTR(EmptyEnv) env = NEW(EmptyEnv)();
        PTR(Expr) returnIf = THIS->interp(env)->to_expr();
        return returnIf;
    }
}

PTR(Expr) IfExpr::substitute(std::string var, PTR(Expr) val)
{
    PTR(IfExpr) returnData = NEW(IfExpr)(this->testExpr->substitute(var, val), this->thenExpr->substitute(var, val), this->elseExpr->substitute(var, val));
    return returnData;
}

bool IfExpr::canInterp()
{
    return (testExpr->canInterp() && thenExpr->canInterp() && elseExpr->canInterp());
}

std::string IfExpr::to_string()
{
    return "_if " + testExpr->to_string() + " _then " + thenExpr->to_string() + " _else " + elseExpr->to_string();  
}

EqualExpr::EqualExpr(PTR(Expr) lhs, PTR(Expr) rhs)
{
    this->lhs = lhs;
    this->rhs = rhs;
}

bool EqualExpr::equals(PTR(Expr) e)
{
    PTR(EqualExpr) i = CAST(EqualExpr)(e);
    if (i == NULL)
    {
        return false;
    }
    else
    {
        return (lhs->equals(i->lhs) && rhs->equals(i->rhs));
    }
}

TEST_CASE( "Equal coverage 1." )
{
    CHECK ( !(NEW(EqualExpr)(NEW(NumExpr)(17), NEW(NumExpr)(17)))->equals(NEW(BoolExpr)(true)));
    CHECK ( (NEW(EqualExpr)(NEW(NumExpr)(17), NEW(NumExpr)(17)))->equals(NEW(EqualExpr)(NEW(NumExpr)(17), NEW(NumExpr)(17))));
}

PTR(Value) EqualExpr::interp(PTR(Env) env)
{
    if (this->lhs->interp(env)->equals(this->rhs->interp(env)))
    {
        return NEW(BoolVal)(true);
    }
    else
    {
        return NEW(BoolVal)(false);
    }
}

void EqualExpr::step_interp()
{
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env;
    Step::cont = NEW(RightThenEqualCont)(rhs, Step::env, Step::cont);
}

PTR(Expr) EqualExpr::optimize()
{
    PTR(EqualExpr) intermediateData = NEW(EqualExpr)(this->lhs->optimize(), this->rhs->optimize());
    if (intermediateData->canInterp())
    {
        PTR(Env) env = NEW(EmptyEnv)();
        PTR(Expr) returnData = intermediateData->interp(env)->to_expr();
        return returnData;
    }
    return intermediateData;
}

PTR(Expr) EqualExpr::substitute(std::string var, PTR(Expr) val)
{
    PTR(EqualExpr) returnData = NEW(EqualExpr)(lhs->substitute(var, val), rhs->substitute(var, val));
    return returnData;
}

bool EqualExpr::canInterp()
{
    return (lhs->canInterp() && rhs->canInterp());
}

std::string EqualExpr::to_string()
{
    return lhs->to_string() + " == " + rhs->to_string();  
}

FunExpr::FunExpr(std::string formalArgument, PTR(Expr) body)
{
    this->formalArgument = formalArgument;
    this->body = body;
}

bool FunExpr::equals(PTR(Expr) e)
{
    PTR(FunExpr) f = CAST(FunExpr)(e);
    if (f == NULL)
    {
        return false;
    }
    else
    {
        return (formalArgument == f->formalArgument) && (body->equals(f->body));
    }
}

TEST_CASE( "Fun coverage 1." )
{
    CHECK ( !(NEW(FunExpr)("x", NEW(AddExpr)(NEW(NumExpr)(1), NEW(VarExpr)("x"))))->equals(NEW(BoolExpr)(true)));
    CHECK ( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(NumExpr)(1), NEW(VarExpr)("x"))))->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(NumExpr)(1), NEW(VarExpr)("x")))));
}

PTR(Value) FunExpr::interp(PTR(Env) env)
{
    return NEW(FunVal)(formalArgument, body, env);
}

void FunExpr::step_interp()
{
    Step::mode = Step::continue_mode;
    Step::val = NEW(FunVal)(formalArgument, body, Step::env);
    Step::cont = Step::cont;
}

PTR(Expr) FunExpr::optimize()
{
    PTR(FunExpr) returnData = NEW(FunExpr)(formalArgument, body->optimize());
    return returnData;
}

PTR(Expr) FunExpr::substitute(std::string var, PTR(Expr) val)
{
    if (var != formalArgument)
    {
        PTR(FunExpr) returnData = NEW(FunExpr)(formalArgument, body->substitute(var, val));
        return returnData;
    }
    else
    {
        return THIS;
    }
}

bool FunExpr::canInterp()
{
    return true;
}

TEST_CASE( "Fun coverage 2." )
{
    CHECK ( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(NumExpr)(1), NEW(VarExpr)("x"))))->canInterp());
}

std::string FunExpr::to_string()
{
    return "_fun (" + formalArgument + ") " + body->to_string();
}

CallExpr::CallExpr(PTR(Expr) toBeCalled, PTR(Expr) actualArg)
{
    this->toBeCalled = toBeCalled;
    this->actualArg = actualArg;
}

bool CallExpr::equals(PTR(Expr) e)
{
    PTR(CallExpr) c = CAST(CallExpr)(e);
    if (c == NULL)
    {
        return false;
    }
    else
    {
        return (toBeCalled->equals(c->toBeCalled)) && (actualArg->equals(c->actualArg));
    }
}

TEST_CASE( "Call coverage 1." )
{
    CHECK ( !(NEW(CallExpr)(NEW(NumExpr)(1), NEW(VarExpr)("x")))->equals(NEW(BoolExpr)(true)));
}

PTR(Value) CallExpr::interp(PTR(Env) env)
{
    return toBeCalled->interp(env)->call(actualArg->interp(env));
}

void CallExpr::step_interp()
{
    Step::mode = Step::interp_mode;
    Step::expr = toBeCalled;
    Step::cont = NEW(ArgThenCallCont)(actualArg, Step::env, Step::cont);  
}

PTR(Expr) CallExpr::optimize()
{
    PTR(CallExpr) returnData = NEW(CallExpr)(toBeCalled->optimize(), actualArg->optimize());
    return returnData;
}

PTR(Expr) CallExpr::substitute(std::string var, PTR(Expr) val)
{
    PTR(CallExpr) returnData = NEW(CallExpr)(toBeCalled->substitute(var, val), actualArg->substitute(var, val));
    return returnData;
}

bool CallExpr::canInterp()
{
    return true;
}

std::string CallExpr::to_string()
{
    return " ( " + toBeCalled->to_string() + " ) ( " + actualArg->to_string() + " ) ";
}

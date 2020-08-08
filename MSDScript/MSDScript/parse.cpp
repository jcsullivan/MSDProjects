//
//  parse.cpp
//  HW3Testing
//
//  Created by Jonathan Sullivan on 1/30/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//
//  parase.cpp takes in an input string from a given source, and attempts to parse the stream
//  into a series of nested expressions that can be interpreted, interpreted by steps, or optimized.

#include <iostream>
#include <sstream>
#include "parse.hpp"

PTR(Expr) parse(std::istream &in);
static PTR(Expr) parseExpression(std::istream &in);
static PTR(Expr) parseComparg(std::istream &in);
static PTR(Expr) parseAddend(std::istream &in);
static PTR(Expr) parseMulticand(std::istream &in);
static PTR(Expr) parseInner(std::istream &in);
static PTR(Expr) parseNumber(std::istream &in);
static PTR(Expr) parseVariable(std::istream &in);
static std::string parseOperator(std::istream &in);
static std::string parseString(std::istream &input, std::string leadingChar);
static PTR(Expr) operatorHandler(std::istream &in);
static char peekNextCharacter(std::istream &in);

/**
 parse(in) is the top-level parsing expresion to be called.  It takes in the stream, peforms first-order error-checking on it, and passes it off to parseExpression, which will actually kick off the expression-building process.  This is the only parsing function that should be called from outside the program.  
 */
PTR(Expr) parse(std::istream &in)
{
    PTR(Expr) expr = parseExpression(in);

    char c = peekNextCharacter(in);

    if ((c != '\n') && !in.eof())
    {
        throw std::runtime_error((std::string)"Expected end of line at " + c + ".");
    }
    return expr;
}

static PTR(Expr) parseExpression(std::istream &in)
{
    PTR(Expr) lhs = parseComparg(in);
    
    PTR(Expr) result = lhs;

    char c = peekNextCharacter(in);

    if (c == '=')
    {
        in.get();
        c = peekNextCharacter(in);
        if (c == '=')
        {
            in.get();
            c = peekNextCharacter(in);  
            PTR(Expr) rhs = parseExpression(in);
            result = NEW(EqualExpr)(lhs, rhs);
        }
        else
        {
            throw std::runtime_error((std::string)"Incomplete equality operator.");
        }
    }

    return result;
}

static PTR(Expr) parseComparg(std::istream &in)
{
    PTR(Expr) lhs = parseAddend(in);
    
    PTR(Expr) result = lhs;
    
    char c = peekNextCharacter(in);
    
    if (c == '+')
    {
        in.get();
        PTR(Expr) rhs = parseComparg(in);
        result = NEW(AddExpr)(lhs, rhs);
    }
    
    return result;
}

static PTR(Expr) parseAddend(std::istream &in)
{
    PTR(Expr) lhs = parseMulticand(in);
    
    PTR(Expr) result = lhs;

    char c = peekNextCharacter(in);

    if (c == '*')
    {
        in.get();
        PTR(Expr) rhs = parseAddend(in);
        result = NEW(MultExpr)(lhs, rhs);
    }

    return result;
}

static PTR(Expr) parseMulticand(std::istream &in)
{
    PTR(Expr) lhs = parseInner(in);
    
    PTR(Expr) result = lhs;
    
    while(peekNextCharacter(in) == '(')
    {
        in.get();
        PTR(Expr) actualArg = parseExpression(in);
        result = NEW(CallExpr)(result, actualArg);
        char c = peekNextCharacter(in);
        if (c != ')')
        {
            throw std::runtime_error((std::string)"Expected ) in function declaration.");
        }
        in.get();
    }
    
    return result;
}

static PTR(Expr) parseInner(std::istream &in) {
    PTR(Expr) expression;
    
    char c = peekNextCharacter(in);

    if ((c == '-') || isdigit(c))
    {
        expression = parseNumber(in);
    }
    else if (c == '(')
    {
        in.get();
        expression = parseExpression(in);
        c = peekNextCharacter(in);
        if (c != ')')
        {
            throw std::runtime_error("Expected a closing parenthesis.");
        }
        in.get();
    }
    else if (isalpha(c))
    {
        expression = parseVariable(in);
    }
    else if (c == '_')
    {
        expression = operatorHandler(in);
    }
    else
    {
        throw std::runtime_error((std::string)"Expected a digit or open parenthesis at " + c + ".");
    }
    
    return expression;
}

static PTR(Expr) parseNumber(std::istream &in)
{
    int returnData;
    in >> returnData;
    return NEW(NumExpr)(returnData);
}

static PTR(Expr) parseVariable(std::istream &in)
{
    return NEW(VarExpr)(parseString(in, ""));
}

static std::string parseOperator(std::istream &in)
{
    in.get();
    return parseString(in, "_");
}

static std::string parseString(std::istream &in, std::string leadingChar)
{
    std::string returnData = leadingChar;
    while(isalpha(in.peek()))
    {
        returnData = returnData + (char)in.get();
    }
    return returnData;
}

static PTR(Expr) operatorHandler(std::istream &in)
{
    std::string firstArg = parseOperator(in);
    if (firstArg == "_true")
    {
        return NEW(BoolExpr)(true);
    }
    else if (firstArg == "_false")
    {
        return NEW(BoolExpr)(false);
    }
    else if (firstArg == "_let")
    {
        char c = peekNextCharacter(in);
        std::string variableName = parseString(in, "");
        c = peekNextCharacter(in);
        
        if (c != '=')
        {
            throw std::runtime_error((std::string)"Expected = at " + c + ".");
        }
        
        in.get();
        c = peekNextCharacter(in);
        PTR(Expr) rhs = parseExpression(in);
        
        c = peekNextCharacter(in);
        std::string secondArg = parseOperator(in);
        
        if (secondArg != "_in")
        {
            throw std::runtime_error((std::string)"Expected _in statement at " + c + ".");
        }
        
        peekNextCharacter(in);
        PTR(Expr) body = parseExpression(in);
        
        return NEW(LetExpr)(variableName, rhs, body);
    }
    else if (firstArg == "_if")
    {
        char c = peekNextCharacter(in);
        PTR(Expr) testExpr = parseExpression(in);
        
        c = peekNextCharacter(in);
        std::string secondArg = parseOperator(in);
        
        if (secondArg != "_then")
        {
            throw std::runtime_error((std::string)"Expected _then statement at " + c + ".");
        }
        
        c = peekNextCharacter(in);
        PTR(Expr) thenExpr = parseExpression(in);
        
        c = peekNextCharacter(in);
        std::string thirdArg = parseOperator(in);
        
        if (thirdArg != "_else")
        {
            throw std::runtime_error((std::string)"Expected _else statement at " + c + ".");
        }
        
        c = peekNextCharacter(in);
        PTR(Expr) elseExpr = parseExpression(in);
        
        return NEW(IfExpr)(testExpr, thenExpr, elseExpr);
    }
    else if (firstArg == "_fun")
    {
        char c = peekNextCharacter(in);
        if (c != '(')
        {
            throw std::runtime_error((std::string)"Expected ( at " + c + ".");
        }
        in.get();
        std::string formalArgument = parseString(in, "");
        c = peekNextCharacter(in);
        if (c != ')')
        {
            throw std::runtime_error((std::string)"Expected ) at " + c + ".");
        }
        in.get();
        c = peekNextCharacter(in);
        PTR(Expr) body = parseExpression(in);
        
        return NEW(FunExpr)(formalArgument, body);
    }
    else
    {
        throw std::runtime_error((std::string)"Unexpected operator: " + firstArg + ".");
    }
}

static char peekNextCharacter(std::istream &in) {
    char c = in.peek();
    if (c == ' ')
    {
        in.get();
        c = peekNextCharacter(in);
    }
    if (c == '\n')
    {
        in.get();
        c = peekNextCharacter(in);
    }
    return c;
}

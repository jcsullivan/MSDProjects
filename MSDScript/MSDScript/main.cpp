//
//  main.cpp
//  HW1ArithmeticParser
//
//  Created by Jonathan Sullivan on 1/10/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//
//  g++ -std=c++17 -o MSDScript *.cpp
//  leaks -atExit -- [filename]
//

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "parse.hpp"

int main (int argc, char *argv[])
{
    PTR(Expr) outputExpression;
    bool optimizeMode = false;
    bool stepMode = false;

    if ((argc > 1) && strcmp(argv[1], "--opt") == 0)
    {
        optimizeMode = true;
        stepMode = false;
        argc--;
        argv++;
    }
    else if((argc > 1) && strcmp(argv[1], "--step") == 0)
    {
        optimizeMode = false;
        stepMode = true;
        argc--;
        argv++;
    }
    else
    {
        optimizeMode = false;
        stepMode = false;
    }
    try
    {
        if (argc > 1)
        {
            std::string filename = argv[1];
            std::ifstream incomingProgram(filename);
            outputExpression = parse(incomingProgram);
        }
        else
        {
            outputExpression = parse(std::cin);
        }
    }
    catch (std::runtime_error error)
    {
        std::cout << error.what() << "\n";
        return 1;
    }
    
    if(optimizeMode)
    {
        std::cout << outputExpression->optimize()->to_string() + "\n";
    }
    else if(stepMode)
    {
        std::cout << Step::interp_by_steps(outputExpression)->to_string() + "\n";
    }
    else
    {
        try
        {
            PTR(EmptyEnv) env = NEW(EmptyEnv)();
            std::cout << outputExpression->interp(env)->to_string() + "\n";
        }
        catch (std::runtime_error error)
        {
            std::cout << error.what() << "\n";
            return 2;
        }
    }
}

//*
//*
//*
//ALL FURTHER FUNCTIONS ARE RESERVED AND USED FOR TESTING PURPOSES.
//*
//*
//*
static PTR(Expr) parse_str(std::string s)
{
    std::istringstream in(s);
    return parse(in);
}

static void parse_str_error(std::string s)
{
    std::istringstream in(s);
    (void)parse(in);
}

static PTR(Value) interp_str(std::string s)
{
    PTR(EmptyEnv) env = NEW(EmptyEnv)();
    std::istringstream in(s);
    return parse(in)->interp(env);
}

static void interp_str_error(std::string s)
{
    PTR(EmptyEnv) env = NEW(EmptyEnv)();
    std::istringstream in(s);
    (void)parse(in)->interp(env);
}

static PTR(Value) interp_step_str(std::string s)
{
    std::istringstream in(s);
    PTR(Expr) parsedExpression = parse(in);
    return Step::interp_by_steps(parsedExpression);
}

static PTR(Expr) optimize_str(std::string s)
{
    std::istringstream in(s);
    return parse(in)->optimize();
}

TEST_CASE( "Expression-ized testing." ) {

    CHECK_THROWS_WITH( parse_str_error(" ( 1 "), "Expected a closing parenthesis." );
    
    CHECK( parse_str("10")->equals(NEW(NumExpr)(10)));
    CHECK( parse_str("(10)")->equals(NEW(NumExpr)(10)));
    CHECK( parse_str("10+1")->equals(NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr)(1))));
    CHECK( parse_str("(10+1)")->equals(NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr)(1))));
    CHECK( parse_str("(10)+1")->equals(NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr) (1))));
    CHECK( parse_str("10+(1)")->equals(NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr) (1))));
    CHECK( parse_str("1+2*3")->equals(NEW(AddExpr)(NEW(NumExpr)(1), NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))));
    CHECK( parse_str("1*2+3")->equals(NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)), NEW(NumExpr)(3))));
    CHECK( parse_str("4*2*3")->equals(NEW(MultExpr)(NEW(NumExpr)(4), NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))));
    CHECK( parse_str("4+2+3")->equals(NEW(AddExpr)(NEW(NumExpr)(4), NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))));
    CHECK( parse_str("4*(2+3)")->equals(NEW(MultExpr)(NEW(NumExpr)(4), NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))));
    CHECK( parse_str("(2+3)*4")->equals(NEW(MultExpr)(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)), NEW(NumExpr)(4))));
    
    CHECK( optimize_str("x")->to_string() == "x");
    CHECK( optimize_str("(x)")->to_string() == "x");
    CHECK( optimize_str("x+1")->to_string() == "( x + 1 )");
    CHECK( optimize_str("(x+1)")->to_string() == "( x + 1 )");
    CHECK( optimize_str("(x)+1")->to_string() == "( x + 1 )");
    CHECK( optimize_str("x+(1)")->to_string() == "( x + 1 )");
    CHECK( optimize_str("xx+yy*zz")->to_string() == "( xx + ( yy * zz ) )");
    CHECK( optimize_str("xxx*yyy+zzz")->to_string() == "( ( xxx * yyy ) + zzz )");
    CHECK( optimize_str("x*y*z")->to_string() == "( x * ( y * z ) )");
    CHECK( optimize_str("xyz+yzx+zxy")->to_string() == "( xyz + ( yzx + zxy ) )");
    CHECK( optimize_str("x*(y+z)")->to_string() == "( x * ( y + z ) )");
    CHECK( optimize_str("(x+y)*z")->to_string() == "( ( x + y ) * z )");
    
    CHECK_THROWS_WITH ( parse_str_error("@"), "Expected a digit or open parenthesis at @." );
    CHECK_THROWS_WITH ( parse_str_error("(1"), "Expected a closing parenthesis." );

    CHECK( parse_str(" 10 ")->equals(NEW(NumExpr)(10)));
    CHECK( parse_str(" (  10 ) ")->equals(NEW(NumExpr)(10)));
    CHECK( parse_str(" 10  + 1")->equals(NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr)(1))));
    CHECK( parse_str(" ( 10 + 1 ) ")->equals(NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr)(1))));
    CHECK( parse_str(" 11 * ( 10 + 1 ) ")->equals(NEW(MultExpr)(NEW(NumExpr)(11), NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr)(1)))));
    CHECK( parse_str(" ( 11 * 10 ) + 1 ")->equals(NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(11), NEW(NumExpr)(10)), NEW(NumExpr)(1))));
    CHECK( parse_str(" 1 + 2 * 3 ")->equals(NEW(AddExpr)(NEW(NumExpr)(1), NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))));
    
    CHECK( optimize_str(" abc ")->to_string() == "abc");
    CHECK( optimize_str(" (  ABC ) ")->to_string() == "ABC");
    CHECK( optimize_str(" ab  + c")->to_string() == "( ab + c )");
    CHECK( optimize_str(" ( abc + 1 ) ")->to_string() == "( abc + 1 )");
    CHECK( optimize_str(" A * ( b + C ) ")->to_string() == "( A * ( b + C ) )");
    CHECK( optimize_str(" ( Ab * 10 ) + c ")->to_string() == "( ( Ab * 10 ) + c )");
    CHECK( optimize_str(" aBc + 2 * XyZ ")->to_string() == "( aBc + ( 2 * XyZ ) )");

    CHECK_THROWS_WITH ( parse_str_error(" @ "), "Expected a digit or open parenthesis at @." );
}

TEST_CASE( "Equals testing." ) {
    CHECK( (NEW(NumExpr)(1))->equals(NEW(NumExpr)(1)) );
    CHECK( ! (NEW(NumExpr)(1))->equals(NEW(NumExpr)(2)) );
    CHECK( ! (NEW(NumExpr)(1))->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(4))) );
}

TEST_CASE( "Value testing." )
{
    CHECK( (NEW(NumVal)(10))->to_string() == "10");
    CHECK( (NEW(NumVal)(10))->to_expr()->equals(NEW(NumExpr)(10)) == true);
    CHECK( (NEW(NumVal)(3))->equals(NEW(NumVal)(3)) == true);
    CHECK( (NEW(NumVal)(3))->equals(NEW(NumVal)(5)) == false);
    CHECK( (NEW(NumVal)(3))->add_to(NEW(NumVal)(4))->equals(NEW(NumVal)(7)) == true);
    CHECK( (NEW(NumVal)(7))->add_to(NEW(NumVal)(4))->equals(NEW(NumVal)(7)) == false);
    CHECK( (NEW(NumVal)(2))->mult_with(NEW(NumVal)(4))->equals(NEW(NumVal)(8)) == true);
}

TEST_CASE( "Substitution testing." )
{
    CHECK ( (NEW(NumExpr)(10)) -> substitute("dog", NEW(NumExpr)(3)) -> equals(NEW(NumExpr)(10)));
    CHECK ( ! (NEW(VarExpr)("cat")) -> substitute("dog", NEW(NumExpr)(3)) -> equals(NEW(NumExpr)(3)));
    CHECK ( (NEW(VarExpr)("Bob")) -> substitute("Bob", NEW(NumExpr)(5)) -> equals(NEW(NumExpr)(5)));
}

TEST_CASE( "Contains variable testing." )
{
    CHECK ( (NEW(NumExpr)(10))->canInterp());
    CHECK ( !(NEW(VarExpr)("fish"))->canInterp());
    CHECK ( (NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2))) -> canInterp());
    CHECK ( !(NEW(AddExpr)(NEW(NumExpr)(1), NEW(VarExpr)("dog"))) -> canInterp());
    CHECK ( (NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2))) -> canInterp());
    CHECK ( !(NEW(MultExpr)(NEW(NumExpr)(1), NEW(VarExpr)("dog"))) -> canInterp());
}

TEST_CASE( "Optimizing." )
{
    CHECK ( optimize_str("2+3")->to_string() == "5");
    CHECK ( optimize_str("2+3*8")->to_string() == "26");
    CHECK ( optimize_str("2+3\n+4")->to_string() == "9");
    CHECK ( optimize_str("x + 3 + 2")->to_string() == "( x + 5 )");
    CHECK ( optimize_str("2 + 3 + x")->to_string() == "( 2 + ( 3 + x ) )");
}

TEST_CASE( "Let-in." )
{
    PTR(EmptyEnv) env;
    CHECK ( optimize_str("_let x = 5 \n _in x + x")->to_string() == "10" );
    CHECK ( optimize_str("_let y = 7 _in (y + 3) * 2")->to_string() == "20");
    CHECK ( optimize_str("_let z = 17 _in x * 2")->to_string() == "( x * 2 )");
    CHECK ( optimize_str("_let x = 2 _in _let y = x + 2 _in y + 2")->to_string() == "6");
    CHECK ( interp_str("_let x = 2 _in _let y = x + 2 _in y + 2")->to_string() == "6");
    CHECK ( interp_str("_let x = 2 _in _let y = x + 2 _in y + 2")->equals(NEW(NumVal)(6)));
    CHECK ( optimize_str("_let z = 2 _in x")->to_string() == "x");
    CHECK ( optimize_str("_let z = 3 _in 17")->to_string() == "17");
    CHECK ( interp_str("_let z = 3 _in 17")->to_string() == "17");
    CHECK ( optimize_str("_let y = 8 _in _let x = 5 _in y")->to_string() == "8");
    CHECK ( interp_str("_let y = 8 _in _let x = 5 _in y")->to_string() == "8");
    CHECK ( interp_str("_let x = (_let y = 1 + 7 _in y+2) _in x+4")->to_string() == "14");
    CHECK ( interp_str("_let x = 5 _in _let x = 6 _in x")->to_string() == "6");
    CHECK_THROWS_WITH ( parse_str_error("_bob x = 5 _in x + x"), "Unexpected operator: _bob." );
    CHECK_THROWS_WITH ( parse_str_error("_let x 5 _in x + x"), "Expected = at 5." );
    CHECK_THROWS_WITH ( parse_str_error("_let x = 5 in x + x"), "Expected _in statement at i." );
    CHECK_THROWS_WITH ( parse_str_error("_let x = 5 _bob x + x"), "Expected _in statement at _." );
}

TEST_CASE( "Booleans.")
{
    PTR(EmptyEnv) env;
    CHECK ( interp_str("_if 1 + 2 == 3 _then 100 _else 0")->to_string() == "100");
    CHECK ( interp_str("_if _true _then 1 _else 2")->to_string() == "1");
    CHECK ( interp_str("_if _false _then 1 _else 2")->to_string() == "2");
    CHECK ( interp_str("_let tuesday = _true _in _if tuesday _then 1 _else 2")->to_string() == "1");
    CHECK ( interp_str("_let tuesday = _true _in _if tuesday _then 1 _else (2 + _false)")->to_string() == "1");
    CHECK ( interp_str("2 + (_if _true _then 1 _else 2)")->to_string() == "3");
    CHECK ( interp_str("_if (_if _false _then _true _else _false) _then 2 _else 3")->to_string() == "3");
    CHECK ( interp_str("_if 5 == 5 _then 1 _else 0")->to_string() == "1");
    CHECK ( interp_str("5 == 5")->to_string() == "_true");
    CHECK ( interp_str("(5 == 5)")->to_string() == "_true");
    CHECK ( interp_str("(5) == (5)")->to_string() == "_true");
    CHECK ( interp_str("_true == _false")->to_string() == "_false");
    CHECK ( interp_str("_true == 5")->to_string() == "_false");
    CHECK ( interp_str("2 == 1 + 1")->to_string() == "_true");
    CHECK ( interp_str("_let x = 2 _in x == 2")->to_string() == "_true");
    CHECK ( interp_str("(2 == 2) == (3 == 3)")->to_string() == "_true");
    CHECK ( interp_str("_true == _true")->to_string() == "_true");
    CHECK ( optimize_str("_true == _false")->to_string() == "_false");
    CHECK ( optimize_str("x + 1 == x + 1")->to_string() == "( x + 1 ) == ( x + 1 )");
    CHECK_THROWS_WITH( interp_str_error("x + 1 == x + 1"), "Free variable: x");
    CHECK_THROWS_WITH( interp_str_error("_true * 55"), "No multiplying booleans.");
    CHECK_THROWS_WITH( interp_str_error("_false + 6"), "No adding booleans.");
}

TEST_CASE( "Functions." )
{
    PTR(EmptyEnv) env;
    CHECK ( interp_str("_let f = _fun (x) x + 1 _in f(10)")->to_string() == "11");
    CHECK ( interp_str("_let f = _fun (x) x*x _in f(2)")->to_string() == "4");
    CHECK ( interp_str("_let y = 8 _in _let f = _fun (x) x*y _in f(2)")->to_string() == "16");
    CHECK ( interp_str("_let f = (_fun (x) (_fun (y) x*x + y*y)) _in (f(2))(3)")->to_string() == "13");
    CHECK ( interp_str("_let add = _fun (x) _fun (y) x + y _in _let addFive = add(5) _in addFive(10)")->to_string() == "15");
    CHECK ( interp_str("(_fun (x) x + 1)(10)")->to_string() == "11");
    CHECK ( interp_str("(_fun (y) 2*2+y*y)(3)")->to_string() == "13");
    CHECK ( parse_str("f(2)(3)")->equals(NEW(CallExpr)(NEW(CallExpr)(NEW(VarExpr)("f"), NEW(NumExpr)(2)), NEW(NumExpr)(3))));
    CHECK ( interp_str("_let factrl = _fun (factrl) _fun (x) _if x == 1 _then 1 _else x * factrl(factrl)(x + -1) _in factrl(factrl)(5)")->to_string() == "120");
    CHECK (parse_str("_let factrl = _fun (factrl) _fun (x) _if x == 1 _then 1 _else x * factrl(factrl)(x + -1) _in _let factorial = _fun (x) factrl(factrl)(x) _in factorial(5)")->to_string() == "120");
    CHECK ( parse_str("_let factrl = _fun (factrl) _fun (x) _if x == 1 _then 1 _else x * factrl(factrl)(x + -1) _in _let factorial = factrl(factrl) _in factorial(5)")->to_string() == "120");
    CHECK ( parse_str("_let fib = _fun (fib) _fun (x) _if x == 0 _then 1 _else _if x == 2 + -1 _then 1 _else fib(fib)(x + -1) + fib(fib)(x + -2) _in fib(fib)(10)")->to_string() == "89");  
    CHECK_THROWS_WITH(parse_str("(_fun (x) x + 1)(10"), "Expected ) in function declaration.");  
}

TEST_CASE( "Suggestions." )
{
    CHECK ( interp_str("_let x = 1 _in _let y = 2 _in _let z = 3 _in x + y + z")->to_string() == "6");
    CHECK ( optimize_str("(2 + 3) + x")->to_string()== "( 5 + x )");
    CHECK ( optimize_str("(2 + 3) + x")->equals(NEW(AddExpr)(NEW(NumExpr)(5), NEW(VarExpr)("x"))));
    CHECK ( optimize_str("(2 + 3)")->equals(NEW(NumExpr)(5)));
    CHECK ( optimize_str("2 + x")->equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x"))));
    PTR(EmptyEnv) env = NEW(EmptyEnv)();
    CHECK( (NEW(LetExpr)("x", NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)), NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))->interp(env)->equals(NEW(NumVal)(25)) );
}

TEST_CASE ( "Coverage." )
{
    PTR(EmptyEnv) env = NEW(EmptyEnv)();
    CHECK ( interp_str("(_fun (x) x + 1) == (_fun (x) x + 1)")->to_string() == "_false");
    CHECK ( interp_str("(_fun (x) x + 1) == 2")->to_string() == "_false");
    CHECK ( interp_str("(_true == _true) == 2")->to_string() == "_false");
    CHECK ( (optimize_str("_let x = 2 _in _if _false _then x _else 2 + 2")->interp(env)->to_string() == "4"));
    CHECK ( (optimize_str("2 * 3")->interp(env)->to_string() == "6"));
    CHECK ( (optimize_str("_true")->to_string() == "_true"));
    CHECK ( (optimize_str("_false")->to_string() == "_false"));
    CHECK ( (optimize_str("(_fun (x) x + 1)(10)")->to_string() == " ( _fun (x) ( x + 1 ) ) ( 10 ) "));
    CHECK ( (optimize_str("_if _true _then x _else y")->to_string() == "_if _true _then x _else y"));
    CHECK ( (optimize_str("_fun (x) x + 1")->interp(env)->to_string() == "[function]"));
    CHECK_THROWS_WITH ( interp_str_error("x + x"), "Free variable: x" );
    CHECK_THROWS_WITH ( interp_str_error("2 == _true"), "Right side of equality is not a number.");
    CHECK_THROWS_WITH ( interp_str_error("2 + _true"), "Right side of add is not a number.");
    CHECK_THROWS_WITH ( interp_str_error("2 * _false"), "Right side of mult is not a number.");
    CHECK_THROWS_WITH ( interp_str_error("(_fun (x) x + 1) + 2"), "No adding functions.");
    CHECK_THROWS_WITH ( interp_str_error("(_fun (x) x + x) * 3"), "No multiplying functions.");
    CHECK_THROWS_WITH ( parse_str_error("_if 2 _else 3"), "Expected _then statement at _.");
    CHECK_THROWS_WITH ( parse_str_error("_if 3 _then 4 else 5"), "Expected _else statement at e.");
    CHECK_THROWS_WITH ( parse_str_error("2 = 3"), "Incomplete equality operator.");
    CHECK_THROWS_WITH ( parse_str_error(" 2 \\"), "Expected end of line at \\.");
    CHECK_THROWS_WITH ( parse_str_error("_fun x x + 1"), "Expected ( at x.");
    CHECK_THROWS_WITH ( parse_str_error("_fun (x x + 1"), "Expected ) at x.");
}

TEST_CASE ( "Continuation / stepping.")
{
    CHECK ( interp_step_str("_let countdown = _fun(countdown) _fun(n) _if n == 0 _then 0 _else countdown(countdown)(n + -1) _in countdown(countdown)(1000000)")->to_string() == "0");
    CHECK ( interp_step_str("_let count = _fun(count) _fun(n) _if n == 0 _then 0 _else 1 + count(count)(n + -1) _in count(count)(100000)")->to_string() == "100000");
    CHECK ( interp_step_str("_let factrl = _fun (factrl) _fun (x) _if x == 1 _then 1 _else x * factrl(factrl)(x + -1) _in factrl(factrl)(5)")->to_string() == "120");
    CHECK ( interp_step_str("_let fib = _fun (fib) _fun (x) _if x == 0 _then 1 _else _if x == 2 + -1 _then 1 _else fib(fib)(x + -1) + fib(fib)(x + -2) _in fib(fib)(10)")->to_string() == "89");
}

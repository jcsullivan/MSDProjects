//
//  environment.hpp
//  HW3Testing
//
//  Created by Jonathan Sullivan on 3/1/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//

#ifndef environment_hpp
#define environment_hpp

#include <stdio.h>
#include <string>
#include "pointer.hpp"

class Value;  

class Env ENABLE_THIS(Env)
{
public:
    virtual PTR(Value) lookup(std::string findName) = 0;
    static PTR(Env) empty;  
};

class EmptyEnv : public Env
{
public:
    EmptyEnv();
    PTR(Value) lookup(std::string findName);
};

class ExtendedEnv : public Env
{
    std::string name;
    PTR(Value) value;
    PTR(Env) rest;
    
public:
    ExtendedEnv(std::string name, PTR(Value) value, PTR(Env) rest);
    PTR(Value) lookup(std::string findName);
};

#endif /* environment_hpp */

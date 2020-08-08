//
//  environment.cpp
//  HW3Testing
//
//  Created by Jonathan Sullivan on 3/1/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//

#include "environment.hpp"

PTR(Env) Env::empty = NEW(EmptyEnv)();

EmptyEnv::EmptyEnv()
{
}

PTR(Value) EmptyEnv::lookup(std::string findName)
{
    throw std::runtime_error("Free variable: " + findName);
}

ExtendedEnv::ExtendedEnv(std::string name, PTR(Value) value, PTR(Env) rest)
{
    this->name = name;
    this->value = value;
    this->rest = rest;
}

PTR(Value) ExtendedEnv::lookup(std::string findName)
{
    ExtendedEnv *env = this;

    while (1)
    {
        if(findName == env->name)
        {
            return env->value;
        }
        else
        {
            ExtendedEnv* extEnv = dynamic_cast<ExtendedEnv*>(env->rest.get());
            if (extEnv == nullptr)
            {
                throw std::runtime_error("Free variable: " + findName);
            }
            else
            {
                env = extEnv;
//                return rest->lookup(findName);
            }
        }
    }
}

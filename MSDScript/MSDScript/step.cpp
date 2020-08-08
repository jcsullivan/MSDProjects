//
//  step.cpp
//  HW3Testing
//
//  Created by Jonathan Sullivan on 3/24/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//

#include "step.hpp"
#include "cont.hpp"
#include "expression.hpp"
#include "environment.hpp"

Step::mode_t Step::mode; 
PTR(Cont) Step::cont;
PTR(Expr) Step::expr;        /* only for Step::interp_mode */
PTR(Value) Step::val;        /* only for Step::continue_mode */
PTR(Env) Step::env;

PTR(Value) Step::interp_by_steps(PTR(Expr) e)
{
    Step::mode = Step::interp_mode;
    Step::expr = e;
    Step::env = Env::empty;
    Step::val = nullptr;
    Step::cont = Cont::done;
    
    while (1) {
        if (Step::mode == Step::interp_mode)
            Step::expr->step_interp();
        else {
            if (Step::cont == Cont::done)
                return Step::val;
            else
                Step::cont->step_continue(); 
        }
    }
}

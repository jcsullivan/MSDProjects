//
//  parse.hpp
//  HW3Testing
//
//  Created by Jonathan Sullivan on 1/30/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//


#ifndef parse_hpp
#define parse_hpp

#include <iostream>
#include "expression.hpp"
#include "step.hpp"

PTR(Expr) parse(std::istream &in);

#endif /* parse_hpp */

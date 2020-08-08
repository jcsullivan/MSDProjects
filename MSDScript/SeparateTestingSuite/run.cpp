//
//  run.cpp
//  In Class Tests
//
//  Created by Jonathan Sullivan on 1/21/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//

extern "C" {
    #include "run.h"
};

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

bool run_tests()
{
    const char *argv[] = { "arith" };
    return (Catch::Session().run(1, argv) == 0);
}

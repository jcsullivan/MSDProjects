//
//  In_Class_Tests.m
//  In Class Tests
//
//  Created by Jonathan Sullivan on 1/21/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//

#import <XCTest/XCTest.h>
#include "run.h"
@interface test : XCTestCase
@end
@implementation test

- (void)testAll {
    if (!run_tests())
    XCTFail(@"failed");
}
@end

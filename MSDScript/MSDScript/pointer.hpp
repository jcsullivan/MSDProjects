//
//  pointer.hpp
//  HW3Testing
//
//  Created by Jonathan Sullivan on 2/28/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//

#ifndef pointer_h
#define pointer_h

#include "catch.hpp"  

#if 0

# define NEW(T)  new T
# define PTR(T)  T*
# define CAST(T) dynamic_cast<T*>
# define THIS    this
# define ENABLE_THIS(T) /* empty */

#else

#include <memory>
# define NEW(T)  std::make_shared<T>
# define PTR(T)  std::shared_ptr<T>
# define CAST(T) std::dynamic_pointer_cast<T>
# define THIS    shared_from_this()
# define ENABLE_THIS(T) : public std::enable_shared_from_this<T>

#endif

#endif /* pointer_h */

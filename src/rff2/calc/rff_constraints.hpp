//
// Created by Merutilm on 10/9/26.
//

#pragma once
#include <cassert>
#ifdef RFF_NO_ASSUME
#define RFF_ASSERT(condition) assert(condition)
#else
#define RFF_ASSERT(condition) \
        do {                     \
            assert(condition);   \
            __builtin_assume(condition); \
        } while (false)
#endif




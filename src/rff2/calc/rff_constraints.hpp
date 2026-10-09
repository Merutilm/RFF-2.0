//
// Created by Merutilm on 10/9/26.
//

#pragma once
#include <cassert>
#define RFF_ASSUME(condition) \
        do {                     \
            assert(condition);   \
            __builtin_assume(condition); \
        } while (false)


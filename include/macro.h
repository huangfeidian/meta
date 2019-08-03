#pragma once

#if defined(__clang__)
    #define Meta(...) __attribute__((annotate(#__VA_ARGS__)))
#else
    #define Meta(...) 
#endif
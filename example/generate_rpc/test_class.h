#pragma once
#include <meta/serialize/property.h>
#include <iostream>
#include <iomanip>
#include <meta/macro.h>
using namespace meta::utils;
namespace test
{
class test_class
{
    Meta(rpc) void rpc_1(int a, std::string b)
    {
        return;
    }
    Meta(rpc) int rpc_2()
    {
        return 1;
    }
#include "test_class.generated_h"
}Meta(rpc);
}

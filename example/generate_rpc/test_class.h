#pragma once
#include <meta/serialize/decode.h>
#include <iostream>
#include <iomanip>
#include <meta/macro.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
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

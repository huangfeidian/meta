#pragma once
#include <iostream>
#include <iomanip>
#include <meta_macro.h>
#include <optional>
#include <vector>
#include <unordered_map>
#include <string>
#include <utility/type_map.h>
#include <utility/string_utils.h>
namespace test
{
class test_class
{
    test_class(std::string& in_str):
    a(in_str),
    b(in_str),
    c(in_str),
    d(in_str)
    {

    }
    Meta(attr(type=func)) void interface_1(int a, std::string b)
    {
        return;
    }
    Meta(attr(type=func)) int interface_2()
    {
        return 1;
    }
	Meta(attr(type=func)) bool interface_3(const std::string& s)
	{
		return false;
	}
    Meta(attr(type=var)) std::string a;
    Meta(attr(type=var)) const std::string b;
    Meta(attr(type=var)) std::string& c;
    Meta(attr(type=var)) const std::string& d;

#include "test_class.generated_h"
}Meta(attr);
}

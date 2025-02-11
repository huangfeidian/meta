#pragma once
#include <iostream>
#include <iomanip>
#include <meta/macro.h>
#include <optional>
#include <vector>
#include <unordered_map>
#include <string>
#include <meta/utility/type_map.h>
#include <meta/utility/string_utils.h>
namespace test
{
class test_class
{
    public:
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
    Meta(attr(type=func)) int interface_2() const
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
#ifndef __meta_parse__
#include "test_class.generated_h"
#endif
}Meta(attr);
class test_class_B: public test_class
{
    test_class_B(std::string& in_str):
    test_class(in_str),
    f("heheh"),
    g(3)
    {

    }
    Meta(attr(type=func)) void interface_4(int a, std::string b)
    {
        return;
    }
    Meta(attr(type=func)) int interface_5() const
    {
        return 1;
    }
	Meta(attr(type=func)) bool interface_6(const std::string& s)
	{
		return false;
	}
    Meta(attr(type=var)) std::string e;
    Meta(attr(type=var)) const std::string f;
    Meta(attr(type=var)) const int g;
#ifndef __meta_parse__
#include "test_class_B.generated_h"
#endif
}Meta(attr);
}

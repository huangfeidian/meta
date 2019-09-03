#pragma once
#include <iostream>
#include <iomanip>
#include <meta_macro.h>
#include <optional>
namespace test
{
class test_class
{
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

#include "test_class.generated_h"
}Meta(attr);
}

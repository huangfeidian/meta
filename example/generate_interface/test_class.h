#pragma once
#include <iostream>
#include <iomanip>
#include <meta_macro.h>
#include <optional>
namespace test
{
class test_class
{
    Meta(interface) void interface_1(int a, std::string b)
    {
        return;
    }
    Meta(interface) int interface_2()
    {
        return 1;
    }
	Meta(interface) bool interface_3(const std::string& s)
	{
		return false;
	}
	void* get_interface_by_name(const std::string& interface_name) const
	{
		return nullptr;
	}
	template<typename T, typename... Args>
	std::optional<T> interface_call(const std::string& interface_name, Args... args)
	{
		auto the_func = get_interface_by_name(interface_name);
		if(!the_func)
		{
			return std::nullopt;
		}
		else
		{
			return (*reinterpret_cast<T(*)(Args...)>(the_func)(std::forward<Args>(args)...);
		}
		
	}
#include "test_class_generated.h"
}Meta(interface);
}

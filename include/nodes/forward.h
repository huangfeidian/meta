#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace meta::language
{
    class variable_node;
    class class_node;
	class callable_node;
	class enum_node;
	void to_json(json& j, const class_node& cur_node);
	void to_json(json& j, const variable_node& cur_node);
	void to_json(json& j, const callable_node& cur_node);
	void to_json(json& j, const enum_node& cur_node);
}
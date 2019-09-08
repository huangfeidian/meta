#include <meta/parser/nodes/forward.h>
#include <meta/parser/nodes/callable.h>
#include <meta/parser/nodes/variable.h>
#include <meta/parser/nodes/class.h>
#include <meta/parser/nodes/enum.h>

namespace meta::language
{
	void to_json(json& j, const class_node& cur_node)
	{
		j = cur_node.to_json();
	}
	void to_json(json& j, const variable_node& cur_node)
	{
		j = cur_node.to_json();
	}
	void to_json(json& j, const callable_node& cur_node)
	{
		j = cur_node.to_json();
	}
	void to_json(json& j, const enum_node& cur_node)
	{
		j = cur_node.to_json();
	}
}
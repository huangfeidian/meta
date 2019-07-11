#include "nodes/forward.h"
#include "nodes/callable.h"
#include "nodes/variable.h"
#include "nodes/class.h"


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
}
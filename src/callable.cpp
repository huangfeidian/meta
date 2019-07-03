#include "nodes/callable.h"

namespace meta::language
{
	callable_node::callable_node(const node* _in_node) :
		node_base(_in_node)
	{
		parse();
	}
	const type_info* callable_node::result_type() const
	{
		return _result_type;
	}
	const std::vector<const variable_node*> callable_node::args_type() const
	{
		return _args;
	}
	bool callable_node::is_class_method() const
	{
		return false;
	}
	bool callable_node::is_static_method() const
	{
		return false;
	}
	bool callable_node::is_public_method() const
	{
		return false;
	}
	bool callable_node::can_accept(const std::vector<const type_info*>& _in_args) const
	{
		return false;
	}
	void callable_node::parse()
	{
		return;
	}
	json callable_node::to_json() const
	{
		json result;
		result["name"] = name();
		result["node_type"] = "callable";
		result["is_class_method"] = is_class_method();
		result["is_static_method"] = is_static_method();
		result["is_public_method"] = is_public_method();
		json arg_type_json;
		for (const auto i : _args)
		{
			arg_type_json.push_back(json(*i));
		}
		result["args_type"] = arg_type_json;
		result["result_type"] = json(*_result_type);
		return result;

	}
}
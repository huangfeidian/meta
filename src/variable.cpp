#include "nodes/variable.h"

namespace meta::language
{
	variable_node::variable_node(const node* _in_node):
		node_base(_in_node),
		_decl_type(type_db::instance().get_type(clang_getCursorType(_in_node->get_cursor())))

	{
		auto& the_logger = utils::get_logger();
		if (_decl_type)
		{
			the_logger.info("get_type {} for variable {}", _decl_type->name(), _in_node->get_name());
		}
		else
		{
			the_logger.info("cant get_type for variable {}", _in_node->get_name());
		}
		
	}
	const type_info* variable_node::decl_type() const
	{
		return _decl_type;
	}
	json variable_node::to_json() const
	{
		json result;
		result["name"] = name();
		result["qualified_name"] = qualified_name();
		result["node_type"] = "variable";
		result["var_type"] = _decl_type->name();
		return result;
	}
	
}
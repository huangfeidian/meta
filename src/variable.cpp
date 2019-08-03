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
			the_logger.info("get_type {} fullname {} canocial type {} for variable {}", _decl_type->name(), utils::full_name(clang_getCursorType(_in_node->get_cursor())), utils::full_name(clang_getCanonicalType(clang_getCursorType(_in_node->get_cursor()))), _in_node->get_name());
		}
		else
		{
			the_logger.warn("cant get_type for variable {}", _in_node->get_name());
		}
		auto argument_childrens = utils::cursor_get_children(_in_node->get_cursor());
		for (const auto& i : argument_childrens)
		{
			if (i.kind >= static_cast<std::uint32_t>(CXCursor_UnexposedExpr) && i.kind <= static_cast<std::uint32_t>(CXCursor_UnaryOperator))
			{
				_has_default_value = true;
			}
			if (i.kind == CXCursor_AnnotateAttr)
			{
				_annotation = utils::parse_annotation(utils::to_string(i));
			}
		}
		
	}
	const type_info* variable_node::decl_type() const
	{
		return _decl_type;
	}
	bool variable_node::has_default_value() const
	{
		return _has_default_value;
	}
	bool variable_node::can_accept(const type_info* in_arg_type) const
	{
		return _decl_type->can_accept_arg_type(in_arg_type);

	}
	json variable_node::to_json() const
	{
		json result;
		result["name"] = name();
		result["qualified_name"] = qualified_name();
		result["node_type"] = "variable";
		result["var_type"] = _decl_type->name();
		result["with_default"] = _has_default_value;
		// result["location"] = get_node()->get_position();
		if (!_annotation.empty())
		{
			result["annotation"] = _annotation;
		}
		return result;
	}
	
}
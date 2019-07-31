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
		return get_node()->get_kind() == CXCursor_CXXMethod;
	}
	bool callable_node::is_static_method() const
	{
		if (is_class_method())
		{
			return clang_CXXMethod_isStatic(get_node()->get_cursor());
		}
		else
		{
			return false;
		}

		
	}
	bool callable_node::is_public_method() const
	{
		return clang_getCXXAccessSpecifier(get_node()->get_cursor()) == CX_CXXPublic;
	}
	bool callable_node::can_accept(const std::vector<const type_info*>& _in_args) const
	{
		if (_in_args.size() > _args.size())
		{
			return false;
		}
		return false;
	}
	void callable_node::parse()
	{
		auto& the_logger = utils::get_logger();
		const auto& _cur_cursor = get_node()->get_cursor();
		std::vector<CXCursor> children = utils::cursor_get_children(_cur_cursor);
		int num_args = clang_Cursor_getNumArguments(_cur_cursor);
		auto _cur_type = clang_getCursorType(_cur_cursor);
		_result_type = type_db::instance().get_type(clang_getResultType(_cur_type));
		for (int i = 0; i < num_args; i++)
		{
			auto arg_cursor = clang_Cursor_getArgument(_cur_cursor, i);
			_args.push_back(new variable_node(node_db::get_instance().create_node(arg_cursor)));
			auto argument_childrens = utils::cursor_get_children(arg_cursor);
			for (const auto& i : argument_childrens)
			{
				if (i.kind >= static_cast<std::uint32_t>(CXCursor_BlockExpr) && i.kind <= static_cast<std::uint32_t>(CXCursor_ParenExpr))
				{
					the_logger.debug("fuction {} argument {} has default value ", name(), utils::to_string(arg_cursor));
					break;
				}
			}
		}
	}
	json callable_node::to_json() const
	{
		json result;
		result["name"] = name();
		result["qualified_name"] = qualified_name();
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
		if (_result_type)
		{
			result["result_type"] = _result_type->to_json();
		}
		// result["location"] = get_node()->get_position();
		return result;

	}
}
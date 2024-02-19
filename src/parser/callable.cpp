#include <meta/parser/nodes/callable.h>

namespace spiritsaway::meta::language
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
	bool callable_node::is_const_method() const
	{
		return clang_CXXMethod_isConst(get_node()->get_cursor());
	}
	std::string callable_node::func_name() const
	{
		auto cur_name = unqualified_name();
		auto paren_iter = cur_name.find('(');
		return cur_name.substr(0, paren_iter);
	}
	bool callable_node::can_accept(const std::vector<const type_info*>& _in_args) const
	{
		auto in_arg_sz = _in_args.size();
		auto req_arg_sz = _args.size();
		if (in_arg_sz > req_arg_sz)
		{
			return false;
		}
		else
		{
			for (std::size_t i = 0; i < in_arg_sz; i++)
			{
				if (!_args[i]->can_accept(_in_args[i]))
				{
					return false;
				}
			}
			if (in_arg_sz < req_arg_sz)
			{
				if (!(_args[in_arg_sz]->has_default_value()))
				{
					return false;
				}
			}
		}
		return true;
	}

	std::string callable_node::func_arg_desc() const
	{
		std::string result = "(";
		for (std::uint32_t i = 0; i < _args.size(); i++)
		{
			auto cur_arg_name = _args[i]->name();
			result += _args[i]->decl_type()->pretty_name();
			result += " ";
			auto temp_iter = cur_arg_name.rfind("::");
			if (temp_iter != std::string::npos)
			{
				cur_arg_name = cur_arg_name.substr(temp_iter + 2);
			}
			result += cur_arg_name;
			if (i + 1 != _args.size())
			{
				result += ", ";
			}

		}
		result += ")";
		return result;
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
		result["args_description"] = func_arg_desc();
		if (!comment().empty())
		{
			result["comment"] = comment();
		}
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
		if (!m_annotation.empty())
		{
			result["annotation"] = m_annotation;
		}
		
		// result["location"] = get_node()->get_position();
		return result;

	}
}
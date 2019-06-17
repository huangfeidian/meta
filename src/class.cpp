#include "nodes/class.h"
#include "nodes/variable.h"
#include "nodes/callable.h"

namespace meta::language
{
	class_node::class_node(const node* _in_node):
		node_base(_in_node)
	{
		auto cur_cursor = _in_node->get_cursor();
		auto cur_cursor_type = clang_getCursorType(cur_cursor);
		_decl_type = type_db::instance().get_type(cur_cursor_type);
	}
	void class_node::parse_fields()
	{
		auto& the_logger = utils::get_logger();
		for (const auto& i : get_node()->get_all_children())
		{
			switch (i->get_kind())
			{
			case CXCursor_FieldDecl:
			{
				auto cur_var_name = i->get_name();
				auto cur_var_node = new variable_node(i);
				auto pre_iter = _fields.find(cur_var_name);
				if (pre_iter != _fields.end())
				{
					the_logger.warn("duplicate variable {} in class {}", cur_var_name, get_node()->get_name());
				}
				the_logger.debug("class {} has variable {} with type {}", get_node()->get_name(), cur_var_name, cur_var_node->decl_type()->name());
				_fields[cur_var_name] = cur_var_node;
				break;
			}
				
			case CXCursor_VarDecl:
			{
				auto cur_var_name = i->get_name();
				auto cur_var_node = new variable_node(i);
				auto pre_iter = _static_fields.find(cur_var_name);
				if (pre_iter != _static_fields.end())
				{
					the_logger.warn("duplicate static variable {} in class {}", cur_var_name, get_node()->get_name());
				}
				the_logger.debug("class {} has static variable {} with type {}", get_node()->get_name(), cur_var_name, cur_var_node->decl_type()->name());
				_static_fields[cur_var_name] = cur_var_node;
				break;
			}
			default:
				break;
			}
		}
	}
	void class_node::parse_methods()
	{
		auto& the_logger = utils::get_logger();
		for (const auto& i : get_node()->get_all_children())
		{
			switch (i->get_kind())
			{
			case CXCursor_CXXMethod:
			{
				auto cur_func_name = i->get_name();
				auto cur_func_node = new callable_node(i);
				the_logger.debug("class {} has func {} with arg_size {}", get_node()->get_name(), cur_func_name, cur_func_node->args_type().size());
				if (clang_CXXMethod_isStatic(i->get_cursor()))
				{
					_static_methods.emplace(cur_func_name, cur_func_node);
				}
				else
				{
					_methods.emplace(cur_func_name, cur_func_node);
				}
				break;
			}

			default:
				break;
			}
		}
	}
}
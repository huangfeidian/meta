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
		auto& the_logger = utils::get_logger();
		if (_decl_type)
		{
			the_logger.info("class {} has type {}", name(), _decl_type->name());
		}
		else
		{
			the_logger.info("class {} fail to construct type", name());
		}
		parse();
		
	}
	void class_node::parse()
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
			case CXCursor_Constructor:
			{
				auto cur_func_node = new callable_node(i);
				the_logger.debug("class {} has constructor with arg_size {}", get_node()->get_name(), cur_func_node->args_type().size());
				_constructors.push_back(cur_func_node);
			}
			case CXCursor_Destructor:
			{
				auto cur_func_node = new callable_node(i);
				the_logger.debug("class {} has destructor", get_node()->get_name());
				_destructor = cur_func_node;
				break;
			}
			case CXCursor_CXXBaseSpecifier:
			{
				auto cur_type_info = type_db::instance().get_type(clang_getCursorType(i->get_cursor()));
				_bases.push_back(cur_type_info);
				break;

			}
			default:
				break;
			}
		}
	}
	bool class_node::is_template() const
	{
		return get_node()->get_kind() == CXCursor_ClassTemplate;
	}
	const std::vector<std::string>& class_node::template_args() const
	{
		return _template_args;
	}
	const type_info* class_node::decl_type() const
	{
		return _decl_type;
	}
	const std::vector<const type_info*> class_node::bases() const
	{
		return _bases;
	}
	const callable_node* class_node::has_method_for(const std::string& _func_name, const std::vector<const type_info*>& _args) const
	{
		auto _candidates = _methods.equal_range(_func_name);
		auto candi_begin = _candidates.first;
		auto candi_end = _candidates.second;
		while (candi_begin != candi_end)
		{
			if (candi_begin->second->can_accept(_args))
			{
				return candi_begin->second;
			}
			candi_begin++;
		}
		return nullptr;
	}
	const callable_node* class_node::has_static_method_for(const std::string& _func_name, const std::vector<const type_info*>& _args) const
	{
		auto _candidates = _static_methods.equal_range(_func_name);
		auto candi_begin = _candidates.first;
		auto candi_end = _candidates.second;
		while (candi_begin != candi_end)
		{
			if (candi_begin->second->can_accept(_args))
			{
				return candi_begin->second;
			}
			candi_begin++;
		}
		return nullptr;
	}
	const variable_node* class_node::has_field(const std::string& _field_name) const
	{
		auto cur_iter = _fields.find(_field_name);
		if (cur_iter == _fields.end())
		{
			return nullptr;
		}
		else
		{
			return cur_iter->second;
		}
	}
	const variable_node* class_node::has_static_field(const std::string& _field_name) const
	{
		auto cur_iter = _static_fields.find(_field_name);
		if (cur_iter == _static_fields.end())
		{
			return nullptr;
		}
		else
		{
			return cur_iter->second;
		}
	}
	const callable_node* class_node::has_constructor_for(const std::vector<const type_info*>& _args) const
	{
		for (const auto& one_item : _constructors)
		{
			if (one_item->can_accept(_args))
			{
				return one_item;
			}
		}
		return nullptr;
	}
}
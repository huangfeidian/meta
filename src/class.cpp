#include "nodes/class.h"
#include "nodes/variable.h"
#include "nodes/callable.h"

namespace meta::language
{
	class_node::class_node(const node* _in_node):
		node_base(_in_node)
	{
		auto& the_logger = utils::get_logger();
		auto cur_cursor = _in_node->get_cursor();
		auto cur_cursor_type = clang_getCursorType(cur_cursor);
		
		the_logger.info("class {} has cursor_type {}", name(), utils::to_string(cur_cursor_type));
		if (cur_cursor.kind == CXCursorKind::CXCursor_ClassTemplate)
		{
			//template classes has invalid cur_cursor_type so actions below has no effect
			_decl_type = type_db::instance().get_type_for_template_class(cur_cursor);
		}
		else
		{
			_decl_type = type_db::instance().get_type(cur_cursor_type);
		}
		
		
		
		if (_decl_type)
		{
			the_logger.info("class {} has type {}", name(), _decl_type->name());
			_decl_type->set_related_class(this);
		}
		else
		{
			the_logger.info("class {} fail to construct type", name());
		}
		parse();
		if (!type_db::instance().add_class(this))
		{
			the_logger.warn("fail to add class {} to type_db", name());
		}
		
	}
	void class_node::parse()
	{
		auto& the_logger = utils::get_logger();
		std::vector<CXCursor> children = utils::cursor_get_children(get_node()->get_cursor());

		for (const auto& i : children)
		{
			auto cur_kind = clang_getCursorKind(i);
			switch (cur_kind)
			{
			case CXCursor_FieldDecl:
			{
				
				auto cur_var_node = new variable_node(node_db::get_instance().create_node(i));
				auto cur_var_name = cur_var_node->name();
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
				
				auto cur_var_node = new variable_node(node_db::get_instance().create_node(i));
				auto cur_var_name = cur_var_node->name();
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
				
				auto cur_func_node = new callable_node(node_db::get_instance().create_node(i));
				auto cur_func_name = cur_func_node->name();
				the_logger.debug("class {} has func {} with arg_size {}", get_node()->get_name(), cur_func_name, cur_func_node->args_type().size());
				if (clang_CXXMethod_isStatic(i))
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
				auto cur_func_node = new callable_node(node_db::get_instance().create_node(i));
				the_logger.debug("class {} has constructor with arg_size {}", get_node()->get_name(), cur_func_node->args_type().size());
				_constructors.push_back(cur_func_node);
			}
			case CXCursor_Destructor:
			{
				auto cur_func_node = new callable_node(node_db::get_instance().create_node(i));
				the_logger.debug("class {} has destructor", get_node()->get_name());
				_destructor = cur_func_node;
				break;
			}
			case CXCursor_CXXBaseSpecifier:
			{
				auto cur_type_info = type_db::instance().get_type(clang_getCursorType(i));
				_bases.push_back(cur_type_info);
				break;

			}
			case CXCursor_TypedefDecl:
			case CXCursor_TypeAliasDecl:

			{
				type_db::instance().get_alias_typedef(i);
				break;
			}
			case CXCursor_TemplateTypeParameter:
			{
				the_logger.debug("get template type arg {}  cursor_type {} for class {}", utils::full_name(i), utils::to_string(clang_getCursorType(i)), name());
				_template_args.push_back(utils::to_string(i));
				utils::template_types::instance().add_type(i);
				break;
			}
			case CXCursor_NonTypeTemplateParameter:
			{
				the_logger.debug("get non type template type arg {}  cursor_type {} for class {}", utils::full_name(i), utils::to_string(clang_getCursorType(i)), name());
				_template_args.push_back(utils::to_string(i));
				break;
			}
			case CXCursor_ClassDecl:
			case CXCursor_ClassTemplate:
			{
				the_logger.debug("{} has subclass {}", name(), utils::to_string(i));
				break;
			}
			case CXCursor_AnnotateAttr:
			{
				_annotation = utils::parse_annotation(utils::to_string(i));
				break;
			}
			default:
				the_logger.debug("visit unknown cursor {} with kind {} in class {}", utils::to_string(i), utils::to_string(cur_kind), name());
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
	const std::vector<const class_node*> class_node::base_classes() const
	{
		std::vector<const class_node*> result;
		for (auto one_base : _bases)
		{
			auto cur_base_class = one_base->related_class();
			if (cur_base_class)
			{
				result.push_back(cur_base_class);
			}
		}
		return result;
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
		for (const auto& i : _bases)
		{
			auto temp_base_class = i->related_class();
			if (temp_base_class)
			{
				auto temp_result = temp_base_class->has_method_for(_func_name, _args);
				if (temp_result)
				{
					return temp_result;
				}
			}
			
		}
		return has_static_method_for(_func_name, _args);
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
		for (const auto& i : _bases)
		{
			auto temp_base_class = i->related_class();
			if (temp_base_class)
			{
				auto temp_result = temp_base_class->has_static_method_for(_func_name, _args);
				if (temp_result)
				{
					return temp_result;
				}
			}

		}
		return nullptr;
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
		for (const auto & one_item : _bases)
		{
			auto _related_class = one_item->related_class();
			if (_related_class)
			{
				auto temp_result = _related_class->has_constructor_for(_args);
				if (temp_result)
				{
					return temp_result;
				}
			}
		}
		return nullptr;
	}
	json class_node::to_json() const
	{
		json result;
		result["name"] = name();
		result["qualified_name"] = qualified_name();
		result["node_type"] = "class";
		json static_fields_json;
		for (const auto i : _static_fields)
		{
			static_fields_json[i.first] = *i.second;

		}
		result["static_fields"] = static_fields_json;
		json fields_json;
		for (const auto i : _fields)
		{
			fields_json[i.first] = *i.second;

		}
		result["fields"] = fields_json;
		json method_json;
		for (const auto& i : _methods)
		{
			method_json[i.first] = *i.second;
		}
		result["methods"] = method_json;
		json static_method_json;
		for (const auto& i : _static_methods)
		{
			static_method_json[i.first] = *i.second;
		}
		result["static_methods"] = static_method_json;
		json bases_json;
		for (const auto& i : _bases)
		{
			bases_json.push_back(i->name());
		}
		result["bases"] = bases_json;
		json constructor_json;
		for (const auto& i : _constructors)
		{
			constructor_json.push_back(*i);
		}
		result["constructors"] = constructor_json;
		result["template_args"] = _template_args;
		// result["location"] = get_node()->get_position();
		if (!_annotation.empty())
		{
			result["annotation"] = _annotation;
		}
		return result;
	}
}
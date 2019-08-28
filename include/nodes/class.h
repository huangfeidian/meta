#pragma once
#include "type_info.h"
#include "forward.h"


namespace meta::language
{
    class class_node: public node_base
    {
    public:
        class_node(const node* _in_node);
		bool is_template() const;
        const std::vector<std::string>& template_args() const;
		const type_info* decl_type() const;
		const std::vector<const type_info*> bases() const;
		const std::vector<const class_node*> base_classes() const;
		const callable_node* has_method_for(const std::string& _func_name, const std::vector<const type_info*>& _args) const;
		const callable_node* has_static_method_for(const std::string& _func_name, const std::vector<const type_info*>& _args) const;
		const variable_node* has_field(const std::string& _field_name) const;
		const variable_node* has_static_field(const std::string& _field_name) const;
		const callable_node* has_constructor_for(const std::vector<const type_info*>& _args) const;
		json to_json() const;
	public:
		template <typename T>
		std::vector<const variable_node*> query_fields_with_pred(const T& _pred) const;
		template <typename T>
		std::vector<const variable_node*> query_fields_with_pred_recursive(const T& _pred) const;
		template <typename T>
		std::vector<const callable_node*> query_method_with_pred(const T& _pred) const;
		template <typename T>
		std::vector<const callable_node*> query_method_with_pred_recursive(const T& _pred) const;
		template <typename T1, typename T2>
		std::unordered_map<const variable_node*, std::vector<const callable_node*>> query_method_recursive_with_fields(const T1& func_pred, const T2& field_pred, bool exclude_self) const;
	private:
		std::vector<std::string> _template_args;
		std::unordered_map<std::string, const variable_node*> _fields;
		std::unordered_map<std::string, const variable_node*> _static_fields;
		std::multimap<std::string, const callable_node*> _methods;
		std::multimap<std::string, const callable_node*> _static_methods;
		std::vector<const callable_node*> _constructors;
		callable_node* _destructor = nullptr;
		std::vector<const type_info*> _bases;
		void parse();
		type_info* _decl_type = nullptr;


	};
	template <typename T>
	std::vector<const variable_node*> class_node::query_fields_with_pred(const T& _pred) const
	{
		std::vector<const variable_node*> result;
		for (const auto& i : _fields)
		{
			if (!i.second)
			{
				continue;
			}
			if (_pred(*i.second))
			{
				result.push_back(i.second);
			}
		}
		return result;
	}
	template <typename T>
	std::vector<const variable_node*> class_node::query_fields_with_pred_recursive(const T& _pred) const
	{
		std::vector<const variable_node*> result = query_fields_with_pred(_pred);
		for (auto one_base : _bases)
		{
			auto one_class = one_base->related_class();
			if (one_class)
			{
				const auto& temp_result = one_class->query_fields_with_pred_recursive(_pred);
				std::copy(temp_result.begin(), temp_result.end(), std::back_inserter(result));
			}

		}
		return result;
		
	}
	template <typename T>
	std::vector<const callable_node*> class_node::query_method_with_pred(const T& _pred) const
	{
		std::vector<const callable_node*> result;
		for (const auto& i : _methods)
		{
			if (!i.second)
			{
				continue;
			}
			if (_pred(*i.second))
			{
				result.push_back(i.second);
			}
		}
		return result;
	}
	template <typename T>
	std::vector<const callable_node*> class_node::query_method_with_pred_recursive(const T& _pred) const
	{
		std::vector<const callable_node*> result = query_method_with_pred(_pred);
		for (auto one_base : _bases)
		{
			auto one_class = one_base->related_class();
			if (one_class)
			{
				const auto& temp_result = one_class->query_method_with_pred_recursive(_pred);
				std::copy(temp_result.begin(), temp_result.end(), std::back_inserter(result));
			}

		}
		return result;

	}
	template <typename T1, typename T2>
	std::unordered_map<const variable_node*, std::vector<const callable_node*>> class_node::query_method_recursive_with_fields(const T1& func_pred, const T2& field_pred, bool exclude_self) const
	{
		std::unordered_map<const variable_node*, std::vector<const callable_node*>> final_result;
		if (!exclude_self)
		{
			auto root_result = query_method_with_pred_recursive(_pred);
			final_result[nullptr] = root_result;
		}
		
		for (const auto one_field : _fields)
		{
			if (!field_pred(*one_field))
			{
				continue;
			}
			auto field_type = one_field->decl_type();
			auto class_type = field_type->related_class();
			if (!class_type)
			{
				continue;
			}
			auto temp_result = class_type->query_method_with_pred_recursive(_pred);
			if (temp_result.size())
			{
				final_result[one_field] = temp_result;
			}
		}
		return final_result;

	}
}
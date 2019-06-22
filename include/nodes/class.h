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
		const callable_node* has_method_for(const std::string& _func_name, const std::vector<const type_info*>& _args) const;
		const callable_node* has_static_method_for(const std::string& _func_name, const std::vector<const type_info*>& _args) const;
		const variable_node* has_field(const std::string& _field_name) const;
		const variable_node* has_static_field(const std::string& _field_name) const;
		const callable_node* has_constructor_for(const std::vector<const type_info*>& _args) const;
	private:
		std::vector<std::string> _template_args;
		std::unordered_map<std::string, const variable_node*> _fields;
		std::unordered_map<std::string, const variable_node*> _static_fields;
		std::multimap<std::string, const callable_node*> _methods;
		std::multimap<std::string, const callable_node*> _static_methods;
		std::vector<const callable_node*> _constructors;
		const callable_node* _destructor;
		std::vector<const type_info*> _bases;
		void parse();
		type_info* _decl_type;


	};
}
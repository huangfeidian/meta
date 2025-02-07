#pragma once
#include "variable.h"

namespace spiritsaway::meta::language
{
    class callable_node: public node_base
    {
    public:
		callable_node(const node* in_node);
        const type_info* result_type() const;
        const std::vector<const variable_node*> args_type() const;
		bool is_class_method() const;
		bool is_static_method() const;
		bool is_public_method() const;
		bool is_const_method() const;
		bool can_accept(const std::vector<const type_info*>& in_args) const;
		bool can_accept(const std::vector<std::string> in_arg_names) const;
		std::string func_name() const;
		json to_json() const;
		std::string func_arg_desc() const;
	private:
		type_info* m_result_type = nullptr;
		std::vector<const variable_node*> m_args;
		void parse();

        
	};
}
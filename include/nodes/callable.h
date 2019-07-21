#pragma once
#include "variable.h"

namespace meta::language
{
    class callable_node: public node_base
    {
    public:
		callable_node(const node* _in_node);
        const type_info* result_type() const;
        const std::vector<const variable_node*> args_type() const;
		bool is_class_method() const;
		bool is_static_method() const;
		bool is_public_method() const;
		bool can_accept(const std::vector<const type_info*>& _in_args) const;
		bool can_accept(const std::vector<std::string> _in_arg_names) const;
		json to_json() const;
	private:
		type_info* _result_type = nullptr;
		std::vector<const variable_node*> _args;
		void parse();

        
	};
}
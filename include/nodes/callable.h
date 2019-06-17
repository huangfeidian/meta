#pragma once
#include "variable.h"

namespace meta::language
{
    class callable_node: public node_base
    {
    public:
		callable_node(const node* _in_node);
        const type_info* result_type() const;
        const std::vector<const type_info*> args_type() const;
		bool is_class_method() const;
		bool is_static_method() const;
		bool is_public_method() const;
	private:
		type_info* _result_type;
		std::vector<const variable_node*> _args;
		void from_class_method();
		void from_class_static_method();
		void from_class_constructor();
		void from_class_destructor();
		void from_method();

        
	};
}
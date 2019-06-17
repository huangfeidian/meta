#pragma once
#include "type_info.h"
#include "forward.h"

namespace meta::language
{
    class variable_node: public node_base
    {
    public:
        variable_node(const node* _in_node);
        const type_info* decl_type() const;
        const std::string& name() const;
    private:
        type_info* _decl_type;
		void from_class_fields();
		void from_class_static_fields();
		void from_func_arg();
	};
}
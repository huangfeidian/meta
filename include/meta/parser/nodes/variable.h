#pragma once
#include "type_info.h"
#include "forward.h"

namespace spiritsaway::meta::language
{
    class variable_node: public node_base
    {
    public:
        variable_node(const node* _in_node);
        const type_info* decl_type() const;
		bool has_default_value() const;
		bool can_accept(const type_info* in_arg_type) const;
		json to_json() const;
    private:
		//type_info* from_class_field(const node* _in_node);
		//type_info* from_class_static_field(const node* _in_node);
		//type_info* from_func_arg(const node* _in_node);
		//type_info* from_global_variable(const node* _in_node);
        type_info* m_decl_type = nullptr;
		bool m_has_default_value = false;
		
	};
}
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
	private:
		std::vector<std::string> _template_args;
		std::unordered_map<std::string, const variable_node*> _fields;
		std::unordered_map<std::string, const variable_node*> _static_fields;
		std::vector<const type_info*> bases;

	};
}
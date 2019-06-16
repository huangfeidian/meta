#pragma once
#include "type_info.h"
#include "forward.h"

namespace meta::language
{
    class variable_node: public node_base
    {
    public:
        variable_node(const node* _in_node);
        const type_info* type_detail() const;
        const std::string& name() const;
    private:
        const type_info* _type_detail;
	};
}
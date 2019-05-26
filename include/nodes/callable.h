#pragma once
#include "variable.h"

namespace meta::language
{
    class callable: public node_base
    {
    public:
        callable(const node* _in_node);
        const type_info* result_type() const;
        const std::vector<const type_info*> args_type() const;
    }
}
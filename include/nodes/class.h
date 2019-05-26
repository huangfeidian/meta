#pragma once
#include "variable.h"

namespace meta::language
{
    class class_node: public node_base
    {
    public:
        class_node(const node* _in_node);
        bool is_template() const;
        const std::vector<const node*>& template_args() const;

    }
}
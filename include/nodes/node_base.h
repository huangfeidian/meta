#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "../node.h"
#include "../name_space.h"
namespace meta::language
{
    class node_base
    {
    public:
        node_base(const node* _in_node);
        const std::string& name() const;
        const std::string& qualified_name() const;
        const node* get_node() const;
        const name_space* get_resident_ns() const;
    private:
        const node* _node;

    }
}

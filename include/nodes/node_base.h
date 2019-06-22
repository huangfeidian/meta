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
		node_base(const node* _in_node):
			_node(_in_node)
		{

		}
		const std::string& name() const
		{
			return _node->get_name();
		}
		const std::string& qualified_name() const
		{
			return _node->get_qualified_name();
		}
		const node* get_node() const
		{
			return _node;
		}
		const name_space* get_resident_ns() const
		{
			return nullptr;
		}
    private:
        const node* _node;

	};
}

#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <nlohmann/json.hpp>
#include "../node.h"
#include "../name_space.h"

using json = nlohmann::json;

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
			return _node->get_qualified_name();
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
		virtual json to_json() const
		{
			json result;
			result["name"] = _node->get_qualified_name();
			result["location"] = _node->get_position();
			return result;
		}
    private:
        const node* _node;

	};
}

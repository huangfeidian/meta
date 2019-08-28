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
	using annotation_map = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;
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
		const std::string& unqualified_name() const
		{
			return _node->get_brief_name();
		}
		const node* get_node() const
		{
			return _node;
		}
		const name_space* get_resident_ns() const
		{
			return nullptr;
		}
		const annotation_map& annotations() const
		{
			return _annotation;
		}
		std::string file() const
		{
			return std::get<0>(_node->get_position());
		}
		virtual json to_json() const
		{
			json result;
			result["name"] = _node->get_qualified_name();
			result["location"] = _node->get_position();
			if (!_annotation.empty())
			{
				result["annotation"] = _annotation;
			}
			return result;
		}

    private:
        const node* _node;
	protected:
		annotation_map _annotation;

	};
}

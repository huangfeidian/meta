#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <nlohmann/json.hpp>
#include <optional>
#include "../node.h"
#include "../name_space.h"
#include "../../utility/string_utils.h"

using json = nlohmann::json;

namespace spiritsaway::meta::language
{
	using annotation_map = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;
    class node_base
    {
    public:
		node_base(const node* _in_node):
			_node(_in_node)
		{
			const auto& _cur_cursor = get_node()->get_cursor();
			std::vector<CXCursor> children = utils::cursor_get_children(_cur_cursor);
			for (auto i : children)
			{
				if (i.kind == CXCursor_AnnotateAttr)
				{
					_annotation = utils::string_utils::parse_annotation(utils::to_string(i));
				}
			}

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
		const std::string& comment() const
		{
			return _node->comment;
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
		std::optional<std::string> get_anotation_detail_value(const std::string& item, const std::string& key) const
		{
			auto item_iter = _annotation.find(item);
			if (item_iter == _annotation.end())
			{
				return std::nullopt;
			}
			auto key_iter = item_iter->second.find(key);
			if (key_iter == item_iter->second.end())
			{
				return std::nullopt;
			}
			return key_iter->second;
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
			auto& temp_comment = comment();
			if (!temp_comment.empty())
			{
				result["comment"] = temp_comment;
			}
			return result;
		}

    private:
        const node* _node;
	protected:
		annotation_map _annotation;

	};
	template <typename T>
	bool filter_with_annotation_value(const std::string& _annotation_name, const std::unordered_map<std::string, std::string>& _annotation_value, const T& _cur_node)
	{

		auto& _cur_annotations = _cur_node.annotations();
		auto cur_iter = _cur_annotations.find(_annotation_name);
		if (cur_iter == _cur_annotations.end())
		{
			return false;
		}
		if (cur_iter->second != _annotation_value)
		{
			return false;
		}
		return true;
	}
	template <typename T>
	bool filter_with_annotation(const std::string& _annotation_name, const T& _cur_node)
	{

		auto& _cur_annotations = _cur_node.annotations();
		auto cur_iter = _cur_annotations.find(_annotation_name);
		if (cur_iter == _cur_annotations.end())
		{
			return false;
		}
		return true;

	}
}

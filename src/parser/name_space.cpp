﻿#include <meta/parser/name_space.h>

namespace spiritsaway::meta::language
{
	std::unordered_map<std::string, name_space*>  name_space::name_space_db = {};
	name_space::name_space(const node* _in_node)
		: qualified_name(_in_node->get_qualified_name())
		, name(_in_node->get_name())
	{
		name_space_db[qualified_name] = this;
		add_synonymous(_in_node);
	}
	bool name_space::add_synonymous(const node* in_node)
	{
		auto cur_iter = synonymouses.insert(in_node);
		return cur_iter.second;
	}
	const name_space::name_space_set& name_space::get_synonymouses() const
	{
		return synonymouses;
	}
	const name_space::name_space_set& name_space::get_synonymous_name_spaces(const std::string& _ns_name)
	{
		static name_space::name_space_set empty;
		auto cur_iter = name_space_db.find(_ns_name);
		if (cur_iter == name_space_db.end())
		{
			return empty;
		}
		else
		{
			return cur_iter->second->get_synonymouses();
		}

	}
	name_space* name_space::get_name_space_for_node(const node* _in_node)
	{
		if(!_in_node)
		{
			return nullptr;
		}
		switch(_in_node->get_kind())
		{
		case CXCursor_Namespace:
			{
				const auto& qualified_name = _in_node->get_qualified_name();
				auto cur_iter = name_space::name_space_db.find(qualified_name);
				if (cur_iter == name_space::name_space_db.end())
				{
					auto temp_ns = new name_space(_in_node);
					return temp_ns;
				}
				else
				{
					cur_iter->second->add_synonymous(_in_node);
					return cur_iter->second;
				}
			}
			break;
		case CXCursor_NamespaceAlias:
			return nullptr;
		case CXCursor_TranslationUnit:
			return nullptr;
		default:
			return get_name_space_for_node(&(_in_node->get_parent()));
			// for normal nodes
			
		}
	}
}
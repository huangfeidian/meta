#include "name_space.h"

namespace meta::language
{
	name_space::name_space(node* _in_node)
		: qualified_name(_in_node->get_qualified_name())
		, name(_in_node->get_name())
	{
		name_space_db[qualified_name] = this;
	}
	bool name_space::add_synonymous(node* in_node)
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
		if (cur_iter != name_space_db.end())
		{
			return empty;
		}
		else
		{
			return cur_iter->second->get_synonymouses();
		}

	}
}
#include "name_space.h"

namespace meta
{
	using std::swap;
	name_space::name_space()
	{

	}
	bool name_space::is_anonymous() const
	{
		return _name.empty();
	}
	bool name_space::is_alias() const
	{
		return _alias_id != _id;
	}
	name_space::name_space(std::uint32_t in_id, const std::string& in_name, std::uint32_t in_alias_id, std::uint32_t in_upper_ns)
	: _id(in_id)
	, _name(in_name)
	, _alias_id(in_alias_id)
	, _upper_ns(in_upper_ns)
	{

	}
	name_space::name_space(const name_space& other)
	{
		_id = other._id;
		_name = other._name;
		_alias_id = other._alias_id;
		_upper_ns = other._upper_ns;
		
	}
	void swap(name_space& a, name_space& b)
	{
		swap(a._id, b._id);
		swap(a._name, b._name);
		swap(a._alias_id, b._alias_id);
		swap(a._upper_ns, b._upper_ns);
	}
	name_space& name_space::operator=(const name_space& other)
	{
		name_space temp(other);
		swap(*this, temp);
		return *this;

	}
	name_space_db& name_space_db::get_instance()
	{
		static name_space_db _instance;
		return _instance;
	}
	name_space_db::name_space_db()
	{
		
		name_space global_ns(0, "", 0, 0);
		all_namespaces[0] = global_ns;
		name_space invalid_ns(1, "invalid", 1, 0);
		all_namespaces[1] = invalid_ns;
		_name_space_counter = 2;
	}
	const name_space& name_space_db::get_name_space(std::uint32_t in_ns_id) const
	{
		auto cur_iter = all_namespaces.find(in_ns_id);
		if(cur_iter == all_namespaces.end())
		{
			return all_namespaces.find(1)->second;
		}
		else
		{
			return cur_iter->second;
		}
		
	}
	const name_space& name_space_db::create_name_space(const std::string& in_name, std::uint32_t in_upper_ns)
	{
		auto& cur_temp_nest_ns = _nest_ns[in_upper_ns];
		for(const auto i : cur_temp_nest_ns)
		{
			const auto& temp_ns = get_name_space(i);
			if(temp_ns._name == in_name)
			{
				return temp_ns;
			}
		}
		auto cur_idx = _name_space_counter++;
		name_space new_name_space(cur_idx, in_name, in_upper_ns, cur_idx);
		all_namespaces[cur_idx] = new_name_space;
		_upper_ns[cur_idx] = in_upper_ns;
		cur_temp_nest_ns.push_back(cur_idx);
		return new_name_space;
	}
	const name_space& name_space_db::create_alias_name_space(const std::string& in_name, std::uint32_t in_upper_ns, std::uint32_t in_alias_ns)
	{
		return name_space();
	}



}
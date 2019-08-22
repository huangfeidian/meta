namespace test
{
	template <typename T>
	std::optional<std::reference_wrapper<T>> entity_base::get_component()
	{
		auto cur_iter = _component_address.find(T::static_name());
		if(cur_iter == _component_address.end())
		{
			return std::nullopt;
		}
		else
		{
			return std::ref(*cur_iter->second);
		}
	}
	template <typename T>
	bool entity_base::has_component() const
	{
		auto cur_iter = _component_address.find(T::static_name());
		if(cur_iter == _component_address.end())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	static const std::string & static_name()
	{
		static std::string cur_class_name = "invalid";
		return cur_class_name;
	}
}
#include "component_base.h"
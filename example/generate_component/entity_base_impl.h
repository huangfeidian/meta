#include "component_base.h"
namespace test
{
	template <typename T>
	std::optional<std::reference_wrapper<T>> entity_base::get_component()
	{
		auto cur_iter = _component_address.find(T::static_type());
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
		auto cur_iter = _component_address.find(T::static_type());
		if(cur_iter == _component_address.end())
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	void entity_base::add_component(component_base* cur_component)
	{
		auto cur_component_type = cur_component->type();
		_component_address[cur_component_type] = cur_component;

	}
}

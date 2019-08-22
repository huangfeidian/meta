#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <optional>
#include <functional>
#include "forward_decl.h"

namespace test
{

	class entity_base
	{
	public:
		entity_type type() const
		{
			return _type;
		}
		static entity_type static_type()
		{
			return entity_type::invalid;
		}
	template <typename T>
	std::optional<std::reference_wrapper<T>> get_component();
	template <typename T>
	bool has_component()const;
	private:
		entity_base(entity_type cur_type):
		_type(cur_type)
		{

		}
		const entity_type _type;
		std::unordered_map<component_type, component_base*> _component_address;
		void add_component(component_base* _cur_component);
	};
}
#include "entity_base_impl.h"


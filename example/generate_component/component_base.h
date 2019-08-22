#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <optional>
#include <functional>

namespace test
{
	class entity_base;
	class component_base
	{
	public:
		component_base(entity_base* in_owner, component_type _in_type):
			_owner(in_owner),
			_type(_in_type)
		{

		}
		template<typename T>
		std::optional<std::reference_wrapper<T>> owner_entity();
		template <typename T>
		bool has_entity() const;
		static component_type static_type()
		{
			return component_type::invalid;
		}
		component_type type() const
		{
			return _type;
		}
	private:
		entity_base* _owner;
		const component_type _type;
	};
}
#include "component_base_impl.h"
#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <optional>
#include <functional>
#include "forward_decl.h"

namespace test
{
	enum class entity_type: std::uint32_t
	{
		invalid = 0,
		player = 1,

	};
	class entity_base
	{
	public:
		entity_type type() const
		{
			return _type;
		}
	template <typename T>
	std::optional<std::reference_wrapper<T>> get_component();
	template <typename T>
	bool has_component()const;
	private:
		const entity_type _type;
		const std::string& _name;
		std::unordered_map<std::string, component_base*> _component_address;
		void add_component(component_base* _cur_component);
	};
}
#include "entity_base_impl.h"


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
		template<typename T>
		std::optional<std::reference_wrapper<T>> get_entity();
	};
}
#include "component_base_impl.h"
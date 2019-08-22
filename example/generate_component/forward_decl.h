#pragma once
#include <cstdint>
namespace test
{
	class entity_base;
	class component_base;
	enum class entity_type: std::uint32_t
	{
		invalid = 0,
		avatar = 1,
	};
	enum class component_type: std::uint32_t
	{
		invalid = 0,
		transform = 1,
		aoi = 2,
		navigate = 3,
		mesh = 4,
	};
	
}
#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <array>
#include <string>
#include <nlohmann/json.hpp>
#include <list>
#include <stack>
#include <deque>
#include <forward_list>
#include <utility>
#include <type_traits>
#include <set>

using json = nlohmann::json;

bool decode(const json& data, float& dst)
{
	if (!data.is_number_float())
	{
		return false;
	}
	else
	{
		dst = data.get<float>();
		return true;
	}
}
bool decode(const json& data, bool& dst)
{
	if (!data.is_boolean())
	{
		return false;
	}
	{
		dst = data.get<bool>();
		return true;
	}
}
bool decode(const json& data, std::string& dst)
{
	if (!data.is_string())
	{

	}
}
template <typename T>
inline typename std::enable_if<std::is_integral<T>::value, bool>::type
decode(const json& data, T& dst)
{
	if (!data.is_number_integer())
	{
		return false;
	}
	else
	{
		dst = data.get<T>();
		return true;
	}
	
}
template<typename T1, typename T2>
bool decode(const json& data, std::pair<T1, T2>& dst)
{
	if (!data.is_array())
	{
		return false;
	}
	if (data.size() != 2)
	{
		return false;
	}
	if (!decode(json[0], dst.first))
	{
		return false;
	}
	if (!decode(json[1], dst.second))
	{
		return false;
	}
	return true;
}
template<typename T>
bool decode(const json& data, std::vector<T>& dst)
{
	if (!data.is_array())
	{
		return false;
	}
	dst.reserve(data.size());
	for (std::size_t i = 0; i < data.size(); i++)
	{
		T temp;
		if (!decode(data[i], temp))
		{
			dst.clear();
			return false;
		}
		else
		{
			dst.push_back(std::move(temp));
		}
	}
	return true;
}
template<typename T>
bool decode(const json& data, std::forward_list<T>& dst)
{
	if (!data.is_array())
	{
		return false;
	}
	for (std::size_t i = 0; i < data.size(); i++)
	{
		T temp;
		if (!decode(data[i], temp))
		{
			dst.clear();
			return false;
		}
		else
		{
			dst.push_front(std::move(temp));
		}
	}
	dst.reverse();
	return true;
}
template<typename T>
bool decode(const json& data, std::list<T>& dst)
{
	if (!data.is_array())
	{
		return false;
	}
	for (std::size_t i = 0; i < data.size(); i++)
	{
		T temp;
		if (!decode(data[i], temp))
		{
			dst.clear();
			return false;
		}
		else
		{
			dst.push_back(std::move(temp));
		}
	}
	dst.reverse();
	return true;
}

template<typename T>
bool decode(const json& data, std::set<T>& dst)
{
	if (!data.is_array())
	{
		return false;
	}
	for (std::size_t i = 0; i < data.size(); i++)
	{
		T temp;
		if (!decode(data[i], temp))
		{
			dst.clear();
			return false;
		}
		else
		{
			dst.insert(std::move(temp));
		}
	}
	dst.reverse();
	return true;
}



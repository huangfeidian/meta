#pragma once
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <string>
#include <cstdint>
#include <optional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define ANY_NUMERIC_CAL(op_name, op_val) template<typename T>	\
bool numeric_cal_##op_name(const T& other_value)	\
{																\
	auto& cur_any_value = *this;								\
	if (!cur_any_value.is_numeric())							\
	{															\
		return false;									\
	}															\
	if (cur_any_value.is_float())								\
	{															\
		float& pre = std::get<float>(cur_any_value);			\
		pre = static_cast<float>(pre op_val other_value);		\
		return true;											\
	}															\
	else if (cur_any_value.is_double())							\
	{															\
		double& pre = std::get<double>(cur_any_value);			\
		pre = static_cast<double>(pre op_val other_value);		\
		return true;											\
	}															\
	else if (cur_any_value.is_int())							\
	{															\
		int& pre = std::get<int>(cur_any_value);			\
		pre = static_cast<int>(pre op_val other_value);		\
		return true;											\
	}															\
	else														\
	{															\
		std::int64_t& pre = std::get<int64_t>(cur_any_value);			\
		pre = static_cast<int64_t>(pre op_val other_value);		\
		return true;											\
	}															\
	return false;									\
}																\

#define ANY_NUMERIC_ANY_CAL(op_name)							\
bool numeric_cal_##op_name(const any_value_type& other_value)	\
{																\
	auto& cur_any_value = *this;								\
	if (!other_value.is_numeric())								\
	{															\
		return false;											\
	}															\
	if (other_value.is_float())									\
	{															\
		return numeric_cal_##op_name(std::get<float>(other_value));\
	}															\
	else if (other_value.is_int())								\
	{															\
		return numeric_cal_##op_name(std::get<int>(other_value));\
	}															\
	else if (other_value.is_double())							\
	{															\
		return numeric_cal_##op_name(std::get<double>(other_value)); \
	}															\
	else if (other_value.is_int64())							\
	{															\
		return numeric_cal_##op_name(std::get<std::int64_t>(other_value));\
	}															\
	return false;												\
}																\

namespace meta::serialize
{
    using any_key_type = std::variant<std::string, int>;
    class any_value_type;
	using any_vector = std::vector<any_value_type>;
	using any_int_map = std::unordered_map<int, any_value_type>;
	using any_str_map = std::unordered_map<std::string, any_value_type>;
    class any_value_type: public std::variant<int, std::int64_t, float, double, bool, std::string, any_vector, any_int_map, any_str_map>
    {
	public:
		using base = std::variant<int, std::int64_t, float, double, bool, std::string, any_vector, any_int_map, any_str_map>;
        using base::base;
        using base::operator=;
		
		bool is_int_map() const
		{
			return std::holds_alternative<any_int_map>(*this);
		}
		bool is_str_map() const
		{
			return std::holds_alternative<any_str_map>(*this);
		}
		bool is_int() const
		{
			return std::holds_alternative<int>(*this);
		}
		bool is_int64() const
		{
			return std::holds_alternative<std::int64_t>(*this);
		}
		bool is_float() const
		{
			return std::holds_alternative<float>(*this);
		}
		bool is_double() const
		{
			return std::holds_alternative<double>(*this);
		}
		bool is_bool() const
		{
			return std::holds_alternative<bool>(*this);
		}
		bool is_str() const
		{
			return std::holds_alternative<std::string>(*this);
		}
		bool is_vector() const
		{
			return std::holds_alternative<any_vector>(*this);
		}
		bool is_numeric() const
		{
			return is_int() || is_int64() || is_float() || is_double();
		}
		template <typename T>
		std::optional<T> numeric_value() const
		{
			if (!is_numeric())
			{
				return std::nullopt;
			}
			else
			{
				T result;
				std::visit([&result](auto&& v)
					{
						result = static_cast<T>(v);
					}, *this);
				return result;

			}
		}
		template <typename T>
		std::optional<bool> numeric_larger_than_impl(const any_value_type& other_value) const
		{
			const auto& cur_any_value = *this;
			if (!cur_any_value.is_numeric() || !other_value.is_numeric())
			{
				return std::nullopt;
			}
			if (!std::holds_alternative<T>(cur_any_value))
			{
				return std::nullopt;
			}
			const auto& cur_raw_value = std::get<T>(cur_any_value);
			if (other_value.is_double())
			{
				return cur_raw_value > std::get<double>(other_value);
			}
			else if (other_value.is_float())
			{
				return cur_raw_value > std::get<float>(other_value);
			}
			else if (other_value.is_int())
			{
				return cur_raw_value > std::get<int>(other_value);
			}
			else if (other_value.is_int64())
			{
				return cur_raw_value > std::get<std::int64_t>(other_value);
			}
			return false;

		}

		std::optional<bool> numeric_larger_than(const any_value_type& other_value) const
		{
			const auto& cur_any_value = *this;
			if (!cur_any_value.is_numeric() || !other_value.is_numeric())
			{
				return std::nullopt;
			}
			if (cur_any_value.is_float())
			{
				return cur_any_value.numeric_larger_than_impl<float> (other_value);
			}
			else if (cur_any_value.is_double())
			{
				return cur_any_value.numeric_larger_than_impl<double>(other_value);
			}
			else if (cur_any_value.is_int())
			{
				return cur_any_value.numeric_larger_than_impl<int>(other_value);
			}
			else
			{
				return cur_any_value.numeric_larger_than_impl<std::int64_t>(other_value);
			}
			
		}
		std::optional<bool> numeric_less_than(const any_value_type& other_value) const
		{
			return other_value.numeric_larger_than(*this);

		}
		std::optional<bool> numeric_no_larger_than(const any_value_type& other_value) const
		{
			const auto& cur_any_value = *this;
			if (!cur_any_value.is_numeric() || !other_value.is_numeric())
			{
				return std::nullopt;
			}
			if (*this == other_value)
			{
				return true;
			}
			return numeric_less_than(other_value);
		}
		std::optional<bool> numeric_no_less_than(const any_value_type& other_value) const
		{
			const auto& cur_any_value = *this;
			if (!cur_any_value.is_numeric() || !other_value.is_numeric())
			{
				return std::nullopt;
			}
			if (*this == other_value)
			{
				return true;
			}
			return numeric_larger_than(other_value);
		}
ANY_NUMERIC_CAL(add, +)
ANY_NUMERIC_CAL(dec, -)
ANY_NUMERIC_CAL(multiply, *)
ANY_NUMERIC_CAL(div, /)
		
ANY_NUMERIC_ANY_CAL(add)
ANY_NUMERIC_ANY_CAL(dec)
ANY_NUMERIC_ANY_CAL(multiply)
ANY_NUMERIC_ANY_CAL(div)
	
	};
	template <typename T>
	typename std::enable_if< std::is_constructible_v<any_value_type, T>, any_value_type>::type 
		any_encode(const T& _in_value)
	{
		return any_value_type(_in_value);
	}
	template <typename T>
	typename std::enable_if< std::is_constructible_v<any_value_type, T>, any_value_type>::type 
		any_encode(const std::vector<T>& _in_value)
	{
		any_vector result;
		for (const auto& one_item : _in_value)
		{
			result.emplace_back(one_item);
		}
		return result;
	}
	template <typename T>
	typename std::enable_if< !std::is_constructible_v<any_value_type, T>, any_value_type>::type 
		any_encode(const std::vector<T>& _in_value)
	{
		any_vector result;
		for (const auto& one_item : _in_value)
		{
			result.emplace_back(any_encode(one_item));
		}
		return result;
	}
	template <typename T>
	typename std::enable_if< std::is_constructible_v<any_value_type, T>, any_value_type>::type 
		any_encode(const std::list<T>& _in_value)
	{
		any_vector result;
		for (const auto& one_item : _in_value)
		{
			result.emplace_back(one_item);
		}
		return result;
	}
	template <typename T>
	typename std::enable_if< !std::is_constructible_v<any_value_type, T>, any_value_type>::type 
		any_encode(const std::list<T>& _in_value)
	{
		any_vector result;
		for (const auto& one_item : _in_value)
		{
			result.emplace_back(any_encode(one_item));
		}
		return result;
	}
	template <typename T>
	typename std::enable_if< std::is_constructible_v<any_value_type, T>, any_value_type>::type
		any_encode(const std::unordered_set<T>& _in_value)
	{
		any_vector result;
		for (const auto& one_item : _in_value)
		{
			result.emplace_back(one_item);
		}
		return result;
	}
	template <typename T>
	typename std::enable_if< !std::is_constructible_v<any_value_type, T>, any_value_type>::type
		any_encode(const std::unordered_set<T>& _in_value)
	{
		any_vector result;
		for (const auto& one_item : _in_value)
		{
			result.emplace_back(any_encode(one_item));
		}
		return result;
	}

	template <typename T>
	typename std::enable_if< std::is_constructible_v<any_value_type, T>, any_value_type>::type 
		any_encode(const std::unordered_map<int, T>& _in_value)
	{
		any_int_map result;
		for (const auto& one_item : _in_value)
		{
			result.emplace(one_item.first, one_item.second);
		}
		return result;
	}
	template <typename T>
	typename std::enable_if< !std::is_constructible_v<any_value_type, T>, any_value_type>::type 
		any_encode(const std::unordered_map<int, T>& _in_value)
	{
		any_int_map result;
		for (const auto& one_item : _in_value)
		{
			result.emplace(one_item.first, any_encode(one_item.second));
		}
		return result;
	}

	template <typename T>
	typename std::enable_if< std::is_constructible_v<any_value_type, T>, any_value_type>::type 
		any_encode(const std::unordered_map<std::string, T>& _in_value)
	{
		any_str_map result;
		for (const auto& one_item : _in_value)
		{
			result.emplace(one_item.first, one_item.second);
		}
		return result;
	}
	template <typename T>
	typename std::enable_if< !std::is_constructible_v<any_value_type, T>, any_value_type>::type 
		any_encode(const std::unordered_map<std::string, T>& _in_value)
	{
		any_str_map result;
		for (const auto& one_item : _in_value)
		{
			result.emplace(one_item.first, any_encode(one_item.second));
		}
		return result;
	}
	template <typename T1, typename T2>
	any_value_type any_encode(const std::pair<T1, T2>& _in_value)
	{
		any_vector result;
		result.push_back(any_encode(_in_value.first));
		result.push_back(any_encode(_in_value.second));
		return result;
	}
	template <typename... Args, std::size_t... index>
	any_value_type any_encode_tuple(const std::tuple<Args...>& _in_value, std::index_sequence<index...>)
	{
		any_vector result;
		(result.push_back(any_encode(std::get<index>(_in_value))),...);
		return result;
	}
	template <typename... Args>
	any_value_type any_encode(const std::tuple<Args...>& _in_value)
	{
		return any_encode_tuple(_in_value, std::index_sequence_for<Args...>{});
	}

	static any_value_type any_encode(const json& data)
	{
		if (data.is_null())
		{
			return any_value_type();
		}
		else if (data.is_boolean())
		{
			return data.get<bool>();
		}
		else if (data.is_number_float())
		{
			return data.get<float>();
		}
		else if (data.is_number_integer())
		{
			return data.get<int>();
		}
		else if (data.is_number_unsigned())
		{
			return data.get<std::int64_t>();
		}
		else if (data.is_array())
		{
			auto result = any_vector();
			for (const auto& one_item : data)
			{
				result.push_back(any_encode(one_item));
			}
			return result;
		}
		else if (data.is_object())
		{
			any_str_map result;
			for (auto& one_item : data.items())
			{
				auto& item_value = one_item.value();
				result[one_item.key()] = any_encode(item_value);

			}
			return result;
		}
		return any_value_type();
	}
	static bool any_decode(const any_value_type& data, any_value_type& dst)
	{
		dst = data;
		return true;
	}
	template <typename T>
	typename std::enable_if<std::is_constructible_v<any_value_type, T>, bool>::type
		any_decode(const any_value_type& data, T& dst)
	{
		if (std::holds_alternative<T>(data))
		{
			dst = std::get<T>(data);
			return true;
		}
		else
		{
			return false;
		}
	}
	template <typename T>
	bool any_decode(const any_value_type& data, std::vector<T>& dst)
	{
		if (!std::holds_alternative<any_vector>(data))
		{
			return false;
			
		}
		auto& cur_vec = std::get<any_vector>(data);
		for (auto& one_item : cur_vec)
		{
			T temp;
			if (any_decode(one_item, temp))
			{
				dst.push_back(temp);
			}
			else
			{
				return false;
			}
		}
		return true;
	}
	template <typename T, std::size_t N>
	bool any_decode(const any_value_type& data, std::array<T, N>& dst)
	{
		if (!std::holds_alternative<any_vector>(data))
		{
			return false;
		}
		auto& cur_vec = std::get<any_vector>(data);
		if (cur_vec.size() != N)
		{
			return false;
		}
		for (auto& one_item : cur_vec)
		{
			T temp;
			if (any_decode(one_item, temp))
			{
				dst.push_back(temp);
			}
			else
			{
				return false;
			}
		}
		return true;
	}
	template <typename T>
	bool any_decode(const any_value_type& data, std::unordered_map<std::string, T>& dst)
	{
		if (!std::holds_alternative<any_str_map>(data))
		{
			return false;
		}
		auto& cur_map = std::get<any_str_map>(data);
		for (auto& one_item : cur_map)
		{
			T temp;
			if (any_decode(one_item.second, temp))
			{
				dst[one_item.first] = temp;
			}
			else
			{
				return false;
			}
		}
		return true;
	}
	template <typename T>
	bool any_decode(const any_value_type& data, std::unordered_map<int, T>& dst)
	{
		if (!std::holds_alternative<any_str_map>(data))
		{
			return false;
		}
		auto& cur_map = std::get<any_str_map>(data);
		for (auto& one_item : cur_map)
		{
			T temp;
			if (any_decode(one_item.second, temp))
			{
				dst[one_item.first] = temp;
			}
			else
			{
				return false;
			}
		}
		return true;
	}

	template <typename T1, typename T2>
	bool any_decode(const any_value_type& data, std::pair<T1, T2>& dst)
	{
		if (!std::holds_alternative<any_vector>(data))
		{
			return false;
		}
		auto& cur_vec = std::get<any_vector>(data);
		if (cur_vec.size() != 2)
		{
			return false;
		}
		if (!any_decode(cur_vec[0], dst.first))
		{
			return false;
		}
		if (!any_decode(cur_vec[1], dst.second))
		{
			return false;
		}
		return true;
	}
	template <typename... Args>
	bool any_decode(const any_value_type& data, std::tuple<Args...>& dst)
	{
		if (!std::holds_alternative<any_vector>(data))
		{
			return false;
		}
		auto& cur_vec = std::get<any_vector>(data);
		if (cur_vec.size() != std::tuple_size<std::tuple<Args...>>::value)
		{
			return false;
		}
	}
	template <typename... Args, std::size_t... index>
	bool decode_for_tuple(const any_vector& data, std::tuple<Args...>& dst, std::index_sequence<index...>)
	{
		return (any_decode(data[index], std::get<index>(dst)) && ...);
	}

	static bool any_decode(const any_value_type& data, json& dst)
	{
		if (data.is_bool())
		{
			dst = std::get<bool>(data);
			return true;
		}
		else if (data.is_str())
		{
			dst = std::get<std::string>(data);
			return true;
		}
		else if (data.is_float())
		{
			dst = std::get<float>(data);
			return true;
		}
		else if (data.is_double())
		{
			dst = std::get<double>(data);
			return true;
		}
		else if (data.is_int())
		{
			dst = std::get<int>(data);
			return true;
		}
		else if (data.is_int64())
		{
			dst = std::get<std::int64_t>(data);
			return true;
		}
		else if (data.is_vector())
		{
			const auto& cur_vec = std::get<any_vector>(data);
			for (auto& one_item : cur_vec)
			{
				json temp;
				any_decode(one_item, temp);
				dst.push_back(temp);
			}
			return true;
		}
		else if (data.is_str_map())
		{
			const auto& cur_vec = std::get<any_str_map>(data);
			for (auto& one_item : cur_vec)
			{
				json temp;
				any_decode(one_item.second, temp);
				dst[one_item.first] = temp;
			}
			return true;
		}
		else if (data.is_int_map())
		{
			const auto& cur_vec = std::get<any_int_map>(data);
			for (auto& one_item : cur_vec)
			{
				json temp;
				any_decode(one_item.second, temp);
				dst[one_item.first] = temp;
			}
			return true;
		}
		else
		{
			return true;
		}
	}
}


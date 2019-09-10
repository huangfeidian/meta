#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <string>
#include <cstdint>

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
		bool is_vector() const
		{
			return std::holds_alternative<any_vector>(*this);
		}
	};
	template <typename T>
	typename std::enable_if< std::is_constructible_v<any_value_type, T>, any_value_type>::type 
		any_convert(const T& _in_value)
	{
		return any_value_type(_in_value);
	}
	template <typename T>
	typename std::enable_if< std::is_constructible_v<any_value_type, T>, any_value_type>::type 
		any_convert(const std::vector<T>& _in_value)
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
		any_convert(const std::vector<T>& _in_value)
	{
		any_vector result;
		for (const auto& one_item : _in_value)
		{
			result.emplace_back(any_convert(one_item));
		}
		return result;
	}
	template <typename T>
	typename std::enable_if< std::is_constructible_v<any_value_type, T>, any_value_type>::type 
		any_convert(const std::list<T>& _in_value)
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
		any_convert(const std::list<T>& _in_value)
	{
		any_vector result;
		for (const auto& one_item : _in_value)
		{
			result.emplace_back(any_convert(one_item));
		}
		return result;
	}
	template <typename T>
	typename std::enable_if< std::is_constructible_v<any_value_type, T>, any_value_type>::type
		any_convert(const std::unordered_set<T>& _in_value)
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
		any_convert(const std::unordered_set<T>& _in_value)
	{
		any_vector result;
		for (const auto& one_item : _in_value)
		{
			result.emplace_back(any_convert(one_item));
		}
		return result;
	}

	template <typename T>
	typename std::enable_if< std::is_constructible_v<any_value_type, T>, any_value_type>::type 
		any_convert(const std::unordered_map<int, T>& _in_value)
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
		any_convert(const std::unordered_map<int, T>& _in_value)
	{
		any_int_map result;
		for (const auto& one_item : _in_value)
		{
			result.emplace(one_item.first, any_convert(one_item.second));
		}
		return result;
	}

	template <typename T>
	typename std::enable_if< std::is_constructible_v<any_value_type, T>, any_value_type>::type 
		any_convert(const std::unordered_map<std::string, T>& _in_value)
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
		any_convert(const std::unordered_map<std::string, T>& _in_value)
	{
		any_str_map result;
		for (const auto& one_item : _in_value)
		{
			result.emplace(one_item.first, any_convert(one_item.second));
		}
		return result;
	}


}


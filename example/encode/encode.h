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
template <typename T, typename T2 = void>
struct has_encode_func : std::false_type
{

};

template <typename... args> struct all_encode_able: std::false_type
{

};

template <>
struct all_encode_able<> : std::true_type
{

};

template <typename T1, typename... args>
struct all_encode_able<T1, args...>: std::integral_constant<bool, has_encode_func<T1>::value && all_encode_able<args...>::value>
{

};

template <typename T>
struct has_encode_func<T, std::void_t<std::enable_if<std::is_same<decltype(std::declval<T>().encode()), json>::value>>> : std::true_type
{

};
template <typename T>
struct has_encode_func<T, std::void_t<typename std::enable_if<std::is_integral<T>::value>::type>> : std::true_type
{

};
template <>
struct has_encode_func<float, void> : std::true_type
{

};
template <>
struct has_encode_func<double, void> : std::true_type
{

};
template <>
struct has_encode_func<std::string, void> : std::true_type
{

};


template<typename T1, typename T2>
struct has_encode_func<std::pair<T1, T2>, std::void_t<typename std::enable_if<has_encode_func<T1>::value && has_encode_func<T2>::value>::type>> : std::true_type
{

};
template<typename... args>
struct has_encode_func<std::tuple<args...>, std::void_t<typename std::enable_if<all_encode_able<args...>::value>::type>> :std::true_type
{

};
template<typename T>
struct has_encode_func < std::vector<T>, std::void_t<typename std::enable_if<has_encode_func<T>::value>::type>> : std::true_type
{

};
template<typename T>
struct has_encode_func < std::list<T>, std::void_t<typename std::enable_if<has_encode_func<T>::value>::type>> : std::true_type
{

};
template<typename T>
struct has_encode_func < std::deque<T>, std::void_t<typename std::enable_if<has_encode_func<T>::value>::type>> : std::true_type
{

};
template<typename T>
struct has_encode_func < std::forward_list<T>, std::void_t<typename std::enable_if<has_encode_func<T>::value>::type>> : std::true_type
{

};
template <typename T1, std::size_t T2>
struct has_encode_func<std::array<T1, T2>, std::void_t<typename std::enable_if<has_encode_func<T1>::value>::type>>: std::true_type
{

};
template <typename T1, typename T2>
struct has_encode_func<std::map<T1, T2>, std::void_t<typename std::enable_if<has_encode_func<T1>::value && has_encode_func<T2>::value>::type>>: std::true_type
{

};
template <typename T1, typename T2>
struct has_encode_func<std::unordered_map<T1, T2>, std::void_t<typename std::enable_if<has_encode_func<T1>::value && has_encode_func<T2>::value>::type>>: std::true_type
{

};
template <typename T1>
struct has_encode_func<std::set<T1>, std::void_t<typename std::enable_if<has_encode_func<T1>::value>::type>>: std::true_type
{

};
template <typename T1>
struct has_encode_func<std::unordered_set<T1>, std::void_t<typename std::enable_if<has_encode_func<T1>::value>::type>>: std::true_type
{

};
template <typename T1, typename T2>
struct has_encode_func<std::multimap<T1, T2>, std::void_t<typename std::enable_if<has_encode_func<T1>::value && has_encode_func<T2>::value>::type>>: std::true_type
{

};
template <typename T1, typename T2>
struct has_encode_func<std::unordered_multimap<T1, T2>, std::void_t<typename std::enable_if<has_encode_func<T1>::value && has_encode_func<T2>::value>::type>>: std::true_type
{

};
template <typename T1>
struct has_encode_func<std::multiset<T1>, std::void_t<typename std::enable_if<has_encode_func<T1>::value>::type>>: std::true_type
{

};
template <typename T1>
struct has_encode_func<std::unordered_multiset<T1>, std::void_t<typename std::enable_if<has_encode_func<T1>::value>::type>>: std::true_type
{

};

template <typename T>
inline typename std::enable_if<std::is_same<decltype(std::declval<T>.encode()), json>::value, json>::type encode(const T& data)
{
    return data.encode();
}
template <typename T>
inline typename std::enable_if<std::is_integral<T>::value, T>::type
encode(const T& data)
{
	return data;
}
double encode(const float& data)
{
    return double(data);
}
double encode(const double& data)
{
    return data;
}
std::string encode(const std::string& data)
{
    return data;
}

template <typename T1, typename T2>
json encode(const std::pair<T1, T2>& data)
{

	json cur_array = json::array({ encode(data.first), encode(data.second) });
	return cur_array;
}
template <typename T1, std::size_t T2>
json encode(const std::array<T1, T2>& data)
{
	json cur_array = json::array();
	for (std::size_t i = 0; i < T2; i++)
	{
		cur_array.push_back(encode(data[i]));
	}
	return cur_array;
}
template <typename tuple_type, std::size_t... index>
json encode_tuple(const tuple_type& data, std::index_sequence<index...>)
{
	json cur_array = json::array({ encode(std::get<index>(data))... });
	return cur_array;
}
template <typename... args>
json encode(const std::tuple<args...>& data)
{
	return encode_tuple(data, std::index_sequence_for<args...>{});
}
template <typename T1, typename T2>
json encode(const std::map<T1, T2>& data)
{
	json cur_array = json::array(); 
	for (const auto& i : data)
	{
		cur_array.push_back(std::make_pair<json, json>(encode(i.first), encode(i.second)));
	}
	return cur_array;
}
template <typename T1, typename T2>
json encode(const std::unordered_map<T1, T2>& data)
{
	json cur_array = json::array();
	for (const auto& i : data)
	{
		cur_array.push_back(std::make_pair<json, json>(encode(i.first), encode(i.second)));
	}
	return cur_array;
}
template <typename T1, typename T2>
json encode(const std::multimap<T1, T2>& data)
{
	json cur_array = json::array();
	for (const auto& i : data)
	{
		cur_array.push_back(std::make_pair<json, json>(encode(i.first), encode(i.second)));
	}
	return cur_array;
}
template <typename T1, typename T2>
json encode(const std::unordered_multimap<T1, T2>& data)
{
	json cur_array = json::array();
	for (const auto& i : data)
	{
		cur_array.push_back(std::make_pair<json, json>(encode(i.first), encode(i.second)));
	}
	return cur_array;
}

template <typename T1>
json encode(const std::set<T1>& data)
{
	json cur_array = json::array();
	for (const auto& i : data)
	{
		cur_array.push_back(encode(i));
	}
	return cur_array;
}
template <typename T1>
json encode(const std::unordered_set<T1>& data)
{
	json cur_array = json::array();
	for (const auto& i : data)
	{
		cur_array.push_back(encode(i));
	}
	return cur_array;
}
template <typename T1>
json encode(const std::multiset<T1>& data)
{
	json cur_array = json::array();
	for (const auto& i : data)
	{
		cur_array.push_back(encode(i));
	}
	return cur_array;
}
template <typename T1>
json encode(const std::unordered_multiset<T1>& data)
{
	json cur_array = json::array();
	for (const auto& i : data)
	{
		cur_array.push_back(encode(i));
	}
	return cur_array;
}
template <typename T1, std::size_t T2>
json encode(const T1(&data)[T2])
{
	json cur_array = json::array();
	for (std::size_t i = 0; i< T2; i++)
	{
		cur_array.push_back(encode(data[i]));
	}
	return cur_array;
}
template <typename T>
json encode(const std::list<T>& data)
{
	json cur_array = json::array();
	for (const auto& i : data)
	{
		cur_array.push_back(encode(i));
	}
	return cur_array;
}
template <typename T>
json encode(const std::forward_list<T>& data)
{
	json cur_array = json::array();
	for (const auto& i : data)
	{
		cur_array.push_back(encode(i));
	}
	return cur_array;
}
template <typename T>
json encode(const std::vector<T>& data)
{
	json cur_array = json::array();
	for (const auto& i : data)
	{
		cur_array.push_back(encode(i));
	}
	return cur_array;
}

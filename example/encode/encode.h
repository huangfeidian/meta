#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <array>
#include <string>
#include <nlohman/json>
#include <list>
#include <stack>
#include <deque>
#include <forward_list>

template <typename T, typename B = void>
struct has_encode_func : false_type
{

};

template <typename... args> struct all_encode_able;

template <>
struct all_encode_able<> : std::true_type
{

};

template <typename A, typename... args>
struct all_encode_able<A, args...>: std::integral_constant<bool, has_encode_func<A>::value && all_encode_able<args...>::value>
{

};

template <typename T>
struct has_encode_func<T, std::void_t<decltype(declval<T>().encode())>> : std::true_type
{

};
template <typename T>
struct has_encode_func<T, std::void_t<typename enable_if<is_integral<T>::value>::type>> : std::true_type
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
struct has_encode_func<tuple<args...>, std::void_t<typename std::enable_if<all_encode_able<args...>::value>::type>> :std::true_type
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
struct has_encode_func < std::queue<T>, std::void_t<typename std::enable_if<has_encode_func<T>::value>::type>> : std::true_type
{

};
template<typename T>
struct has_encode_func < std::forward_list<T>, std::void_t<typename std::enable_if<has_encode_func<T>::value>::type>> : std::true_type
{

};
template <typename A, std::size_t B>
struct has_encode_func<std::array<A, B>, std::void_t<typename std::enable_if<has_encode_func<A>::value>: std::true_type
{

};
template<typename A, std::size_t B>
struct has_encode_func<A[B], std::void_t<typename std::enable_if<has_encode_func<A>::value>: std::true_type
{

};
template <typename A, typename B>
struct has_encode_func<std::map<A, B>, std::void_t<typename std::enable_if<has_encode_func<T1>::value && has_encode_func<T2>::value>::type>>: std::true_type
{

};
template <typename A, typename B>
struct has_encode_func<std::unordered_map<A, B>, std::void_t<typename std::enable_if<has_encode_func<T1>::value && has_encode_func<T2>::value>::type>>: std::true_type
{

};
template <typename A>
struct has_encode_func<std::set<A>, std::void_t<typename std::enable_if<has_encode_func<T>>: std::true_type
{

}
template <typename A>
struct has_encode_func<std::unordered_set<A>, std::void_t<typename std::enable_if<has_encode_func<T>>: std::true_type
{

}
template <typename A, typename B>
struct has_encode_func<std::multimap<A, B>, std::void_t<typename std::enable_if<has_encode_func<T1>::value && has_encode_func<T2>::value>::type>>: std::true_type
{

};
template <typename A, typename B>
struct has_encode_func<std::unordered_multimap<A, B>, std::void_t<typename std::enable_if<has_encode_func<T1>::value && has_encode_func<T2>::value>::type>>: std::true_type
{

};
template <typename A>
struct has_encode_func<std::multiset<A>, std::void_t<typename std::enable_if<has_encode_func<T>>: std::true_type
{

}
template <typename A>
struct has_encode_func<std::unordered_multiset<A>, std::void_t<typename std::enable_if<has_encode_func<T>>: std::true_type
{

}

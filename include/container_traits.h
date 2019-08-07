#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <array>
#include <string>
#include <list>
#include <stack>
#include <deque>
#include <forward_list>
#include <utility>
#include <type_traits>
#include <set>

template<template <typename T1> class T2>
struct is_std_set :std::false_type
{

};
template<>
struct is_std_set<std::set> : std::true_type
{

};
template<>
struct is_std_set<std::unordered_set> : std::true_type
{

};
template <>
struct is_std_set<std::unordered_set> :std::true_type
{

};
template <>
struct is_std_set<std::unordered_multiset> : std::true_type
{

};

template<template<typename T1, typename T2, typename T3, typename T4> class T5>
struct is_std_map :std::false_type
{

};
template<>
struct is_std_map<std::map> :std::true_type
{

};
template<>
struct is_std_map<std::unordered_map> :std::true_type
{

};
template<>
struct is_std_map<std::multimap> :std::true_type
{

};
template<>
struct is_std_map<std::unordered_multimap> :std::true_type
{

};
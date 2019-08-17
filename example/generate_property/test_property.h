#pragma once
#include <serialize/decode.h>
#include <iostream>
#include <iomanip>
#include <meta_macro.h>
using var_idx_type = std::uint8_t;// 每个变量在当前定义内只能是uint8的整数索引 所以一个类里面只能定义255个变量 0号变量保留不使用 以后可以扩充为std::uint16_t 或者与var_mutate_cmd合并为一个std::uint16_t 
using var_cmd_type = std::uint8_t;// 对于变量的改变操作类型 全量赋值 清空 等等
const static std::uint8_t depth_max = 8;
using var_prefix_idx_type = std::vector<var_idx_type>;
enum class var_mutate_cmd : var_cmd_type
{
	clear = 0,
	set = 1,
	vector_push_back = 10,
	vector_idx_mutate = 11,
	vector_idx_delete = 12,
	vector_insert_head = 13,
	vector_pop_back = 14,
	map_insert = 20,
	map_erase = 21,
	set_add = 30,
	set_erase = 31,

};
using mutate_msg = std::tuple<var_prefix_idx_type, var_idx_type, var_mutate_cmd, json>;
class test_property
{
public:
	test_property(const test_property* _in_parent, var_prefix_idx_type _in_cur_depth) :
		_parent(_in_parent),
		_cur_depth(_in_cur_depth)
	{

	}
	const test_property* _parent;
	Meta(encode, decode, property) int a;
	Meta(encode, decode, property std::vector<std::string> b;
	Meta(encode, decode, property) std::unordered_map<int, std::string> c;
    Meta(encode, decode, property) std::unordered_set<int> d;
#include "test_property_generated.h"
} Meta(encode(auto), decode(auto), property(auto));
#pragma once
#include <meta/serialize/property.h>
#include <iostream>
#include <iomanip>
#include <meta/macro.h>
namespace test
{
class test_property
{
public:
	test_property(const test_property* _in_parent, meta::serialize::var_prefix_idx_type _in_cur_depth, std::deque<meta::serialize::mutate_msg>& _in_cmd_buffer):
		_parent(_in_parent),
		_cur_depth(_in_cur_depth),
		_cmd_buffer(_in_cmd_buffer)
	{

	}
	
	Meta(encode, decode, property) int a;
	Meta(encode, decode, property) std::vector<std::string> b;
	Meta(encode, decode, property) std::unordered_map<int, std::string> c;
    Meta(encode, decode, property) std::unordered_set<int> d;
	public:
	const test_property* _parent;
	std::deque<meta::serialize::mutate_msg>& _cmd_buffer;
	const meta::serialize::var_prefix_idx_type _cur_depth;
#include "test_property.generated_h"
} Meta(encode(auto), decode(auto), property(auto));
}

#include <serialize/container.h>
#include <iostream>
#include <iomanip>
using namespace std;
using namespace meta::serialize;

class Entity
{
public:
	Entity(const std::string& _in_name) :
		_name(_in_name)

	{

	}
private:
	std::string _name;
};
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
class PropertyMap
{
public:
	PropertyMap(const PropertyMap* _in_parent, var_prefix_idx_type _in_cur_depth) :
		_parent(_in_parent),
		_cur_depth(_in_cur_depth)
	{

	}
	const PropertyMap* _parent;
	int a;
	std::vector<std::string> b;
	std::unordered_map<int, std::string> c;
	const int& a_get() const
	{
		return a;
	}
	mutate_msg a_set(const int& _in_data)
	{
		a = _in_data;
		return std::make_tuple(_cur_depth, index_for_a, var_mutate_cmd::set, encode_multi(a));

	}
	mutate_msg a_clear()
	{
		a = 0;
		return std::make_tuple(_cur_depth, index_for_a, var_mutate_cmd::clear, json::array());
	}
	bool z_replay_a_clear(const json& data)
	{
		a = 0;
		return true;
	}
	bool z_replay_a_set(const json& data)
	{
		return decode(data[0], a);
	}
	bool z_replay_a_mutate_msg(var_mutate_cmd cmd, const json& data)
	{
		switch (cmd)
		{
		case var_mutate_cmd::clear:
			return z_replay_a_clear(data);
		case var_mutate_cmd::set:
			return z_replay_a_set(data);

		default:
			return false;
		}
	}
	const std::vector<std::string>& b_get() const
	{
		return b;
	}
	mutate_msg b_set(const std::vector<std::string>& _in_data)
	{
		b = _in_data;
		return std::make_tuple(_cur_depth, index_for_b, var_mutate_cmd::set, encode_multi(b));
	}
	bool z_replay_b_set(const json& data)
	{
		return decode_multi(data, b);
	}
	mutate_msg b_push_back(const std::string& _in_data)
	{
		b.push_back(_in_data);
		return std::make_tuple(_cur_depth, index_for_b, var_mutate_cmd::vector_push_back, encode_multi(_in_data));
	}
	bool z_replay_b_push_back(const json& data)
	{
		std::string temp;
		if (decode_multi(data, temp))
		{
			b.push_back(temp);
			return true;
		}
		else
		{
			return false;
		}

	}
	mutate_msg b_pop_back()
	{
		if (b.size())
		{
			b.pop_back();
		}

		return std::make_tuple(_cur_depth, index_for_b, var_mutate_cmd::vector_pop_back, json::array());
	}
	bool z_replay_b_pop_back(const json& data)
	{
		if (b.size())
		{
			b.pop_back();
		}
		return true;
	}
	mutate_msg b_idx_mutate(std::size_t idx, const std::string& _in_data)
	{
		if (idx < b.size())
		{
			b[idx] = _in_data;
		}
		return std::make_tuple(_cur_depth, index_for_b, var_mutate_cmd::vector_idx_mutate, encode_multi(idx, _in_data));
	}
	bool z_replay_b_idx_mutate(const json& data)
	{
		std::size_t idx;
		std::string temp;
		if (decode_multi(data, idx, temp))
		{
			if (idx >= b.size())
			{
				return false;
			}
			else
			{
				b[idx] = temp;
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	mutate_msg b_idx_delete(std::size_t idx)
	{
		if (idx < b.size())
		{
			b.erase(b.begin() + idx);
		}
		return std::make_tuple(_cur_depth, index_for_b, var_mutate_cmd::vector_idx_mutate, encode_multi(idx));
	}
	bool z_replay_b_idx_delete(const json& data)
	{
		std::size_t idx;
		if (decode_multi(data, idx))
		{
			if (idx >= b.size())
			{
				return false;
			}
			else
			{
				b.erase(b.begin() + idx);
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	mutate_msg b_clear()
	{
		b.clear();
		return std::make_tuple(_cur_depth, index_for_c, var_mutate_cmd::clear, json::array());
	}
	bool z_replay_b_clear(const json& data)
	{
		b.clear();
		return true;
	}
	bool z_replay_b_mutate_msg(var_mutate_cmd cmd, const json& data)
	{
		switch (cmd)
		{
		case var_mutate_cmd::clear:
			return z_replay_b_clear(data);
		case var_mutate_cmd::set:
			return z_replay_b_set(data);
		case var_mutate_cmd::vector_push_back:
			return z_replay_b_push_back(data);
		case var_mutate_cmd::vector_idx_mutate:
			return z_replay_b_idx_mutate(data);
		case var_mutate_cmd::vector_idx_delete:
			return z_replay_b_idx_delete(data);
		case var_mutate_cmd::vector_pop_back:
			return z_replay_b_pop_back(data);
		default:
			return false;
		}
	}
	const std::unordered_map<int, std::string> c_get() const
	{
		return c;
	}
	mutate_msg c_set(const unordered_map<int, std::string>& _in_data)
	{
		c = _in_data;
		return std::make_tuple(_cur_depth, index_for_c, var_mutate_cmd::set, json(encode_multi(c)));
	}
	bool z_replay_c_set(const json& data)
	{
		return decode_multi(data, c);
	}
	mutate_msg c_insert(int key, const std::string& value)
	{
		c[key] = value;

		return std::make_tuple(_cur_depth, index_for_c, var_mutate_cmd::map_insert, encode_multi(key, value));
	}
	bool z_replay_c_insert(const json& data)
	{
		int key;
		std::string value;
		if (decode_multi(data, key, value))
		{
			c[key] = value;
			return true;
		}
		else
		{
			return false;
		}
	}
	mutate_msg c_clear()
	{
		c.clear();
		return std::make_tuple(_cur_depth, index_for_c, var_mutate_cmd::clear, json::array());
	}
	bool z_replay_c_clear(const json& data)
	{
		c.clear();
		return true;
	}
	mutate_msg c_erase(int key)
	{
		c.erase(key);
		return std::make_tuple(_cur_depth, index_for_c, var_mutate_cmd::map_erase, encode_multi(key));
	}
	bool z_replay_c_erase(const json& data)
	{
		int key;
		if (decode_multi(data, key))
		{
			c.erase(key);
			return true;
		}
		else
		{
			return false;
		}
	}
	bool z_replay_c_mutate_msg(var_mutate_cmd cmd, const json& data)
	{
		switch (cmd)
		{
		case var_mutate_cmd::clear:
			return z_replay_c_clear(data);
		case var_mutate_cmd::set:
			return z_replay_c_set(data);
		case var_mutate_cmd::map_insert:
			return z_replay_c_insert(data);
		case var_mutate_cmd::map_erase:
			return z_replay_c_erase(data);
		default:
			return false;
		}
	}
	bool replay_mutate_msg(std::size_t field_index, var_mutate_cmd cmd, const json& data)
	{
		switch (field_index)
		{
		case index_for_a:
			return z_replay_a_mutate_msg(cmd, data);
		case index_for_b:
			return z_replay_b_mutate_msg(cmd, data);
		case index_for_c:
			return z_replay_c_mutate_msg(cmd, data);
		default:
			return false;
		}
	}
	bool operator==(const PropertyMap& other)
	{
		return a == other.a && b == other.b && c == other.c;
	}
private:
	const var_prefix_idx_type _cur_depth;
	const static var_idx_type index_for_a = 0;
	const static var_idx_type index_for_b = 1;
	const static var_idx_type index_for_c = 2;
};
void test_property_mutate()
{
	std::vector<std::uint8_t> depth = { 1,2,3 };
	PropertyMap test_a(nullptr, depth);
	PropertyMap test_b(nullptr, depth);
	mutate_msg msg;
	msg = test_a.a_set(1);
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	msg = test_a.b_set(std::vector<std::string>{"hehe", "hahah"});
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	msg = test_a.b_push_back("ee");
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	msg = test_a.b_pop_back();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	msg = test_a.c_insert(1, "eh");
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	msg = test_a.c_insert(1, "ahaha");
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	msg = test_a.c_insert(3, "ahaha");
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	msg = test_a.c_erase(3);
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	msg = test_a.c_clear();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

}
int main()
{
	test_property_mutate();
}
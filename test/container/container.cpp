#include <serialize/container.h>
#include <iostream>
#include <iomanip>

using namespace meta::serialize;
using namespace std;
using json = nlohmann::json;

#define size_print(CUR_TYPE) do{cout<<"sizeof "#CUR_TYPE<<" is "<< sizeof(CUR_TYPE)<<std::endl;}while(0)
void container_size_print()
{
	size_print(json);
	size_print(any_key_type);
	size_print(any_value_type);
	size_print(string);
	size_print(any_list);
	size_print(any_map);
	size_print(any_int_map);
	size_print(any_str_map);
	using str_map = unordered_map<string, string>;
	size_print(str_map);
}
template <typename T>
void decode_test_print(const T& data)
{
	T dst;
	json encode_data = encode(data);
	std::cout << std::setw(4) << "encode result " << encode_data << std::endl;
	bool decode_result = decode(encode_data, dst);
	std::cout << "decode result " << decode_result << " with encode result " << encode(dst) << std::endl;
}
void container_encode_print()
{
	any_key_type data_1;
	decode_test_print(data_1);
	any_key_type data_2 = 1;
	decode_test_print(data_2);
	any_key_type data_3 = "hehe";
	decode_test_print(data_3);
	any_value_type data_4;
	decode_test_print(data_4);
	any_value_type data_5 = 1;
	any_value_type data_6 = false;
	any_value_type data_7 = 2.0f;
	any_value_type data_8 = 3.0;
	any_value_type data_9 = "4.0";
	any_value_type data_10 = std::vector<any_value_type>({ data_5, data_6, data_7 });
	any_value_type data_11 = any_map({ { 1, data_6 }, { "4.0", data_7 } });
	decode_test_print(data_5);
	decode_test_print(data_6);
	decode_test_print(data_7);
	decode_test_print(data_8);
	decode_test_print(data_9);
	decode_test_print(data_10);
	decode_test_print(data_11);
}

class Entity
{
public:
	Entity(const std::string& _in_name):
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
	vector_push_back = 2,
	vector_idx_mutate = 3,
	vector_idx_delete = 4,
	vector_insert_head = 5,
	map_insert = 6,
	map_erase = 7,

};
using mutate_msg = std::tuple<var_prefix_idx_type, var_idx_type, var_mutate_cmd, json>;
class PropertyMap
{
public:
	PropertyMap(const PropertyMap* _in_parent, var_prefix_idx_type _in_cur_depth):
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
		return std::make_tuple(_cur_depth, index_for_a, var_mutate_cmd::set, encode(a));

	}
	const std::vector<std::string>& b_get() const
	{
		return b;
	}
	mutate_msg b_set(const std::vector<std::string>& _in_data)
	{
		b = _in_data;
		return std::make_tuple(_cur_depth, index_for_b, var_mutate_cmd::set, encode(b));
	}
	mutate_msg b_push_back(const std::string& _in_data)
	{
		b.push_back(_in_data);
		return std::make_tuple(_cur_depth, index_for_b, var_mutate_cmd::vector_push_back, json(encode(_in_data)));
	}
	mutate_msg b_idx_mutate(std::size_t idx, const std::string& _in_data)
	{
		if (idx < b.size())
		{
			b[idx] = _in_data;
		}
	}
	mutate_msg b_clear()
	{
		b.clear();
		return std::make_tuple(_cur_depth, index_for_c, var_mutate_cmd::clear, json());
	}
	const std::unordered_map<int, std::string> c_get() const
	{
		return c;
	}
	mutate_msg c_set(const unordered_map<int, std::string>& _in_data)
	{
		c = _in_data;
		return std::make_tuple(_cur_depth, index_for_c, var_mutate_cmd::set, json(encode(c)));
	}
	mutate_msg c_insert(int key, const std::string& value)
	{
		c[key] = value;

		return std::make_tuple(_cur_depth, index_for_c, var_mutate_cmd::map_insert, encode_multi(key, value));
	}
	mutate_msg c_clear()
	{
		c.clear();
		return std::make_tuple(_cur_depth, index_for_c, var_mutate_cmd::clear, json());
	}
	mutate_msg c_erase(int key)
	{
		c.erase(key);
		return std::make_tuple(_cur_depth, index_for_c, var_mutate_cmd::map_erase, encode(key));
	}
private:
	const var_prefix_idx_type _cur_depth;
	const static var_idx_type index_for_a = 0;
	const static var_idx_type index_for_b = 1;
	const static var_idx_type index_for_c = 2;
};
int main()
{
	container_size_print();
	container_encode_print();
}
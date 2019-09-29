#include <meta/serialize/any_value.h>
#include <meta/serialize/decode.h>
#include <iostream>

using namespace std;
using namespace meta::serialize;
void debug_show(const any_value_type& cur_value)
{
	cout << "current any value is " << encode(cur_value) << endl;
	if (cur_value.is_int64())
	{
		cout << "type is int64" << endl;
	}
	if (cur_value.is_double())
	{
		cout << "type is is_double" << endl;
	}
	if (cur_value.is_str())
	{
		cout << "type is is_str" << endl;
	}
	if (cur_value.is_vector())
	{
		cout << "type is is_vector" << endl;
		auto& vec_value = std::get<any_vector>(cur_value);
		for (const auto& one_item : vec_value)
		{
			debug_show(one_item);
		}
	}
	if (cur_value.is_int_map())
	{
		cout << "type is is_int_map" << endl;
		for (const auto& one_item : std::get<any_int_map>(cur_value))
		{
			std::cout << "key " << one_item.first<<" ";
			debug_show(one_item.second);
		}
	}
	if (cur_value.is_str_map())
	{
		cout << "type is is_str_map" << endl;
		for (const auto& one_item : std::get<any_str_map>(cur_value))
		{
			std::cout << "key " << one_item.first<<" ";
			debug_show(one_item.second);
		}
	}
}
int main()
{
	any_int_map int_map;
	std::cout << "the size of any is " << sizeof(any_value_type) << std::endl;
	std::cout << "the size of any_vec is " << sizeof(any_vector) << std::endl;
	std::cout << "the size of any_int_map is " << sizeof(any_int_map) << std::endl;
	std::cout << "the size of any_str_map is " << sizeof(any_str_map) << std::endl;
	int_map[1] = "string";
	int_map[2] = 1.0f;
	int_map[2] = 4;
	int_map[3] = false;
	std::vector<string> str_vec;
	str_vec.push_back("str1");
	str_vec.push_back("str2");
	int_map[4] = any_encode(str_vec);
	std::unordered_map<std::string, int> temp_map;
	temp_map["str1"] = 1;
	temp_map["str2"] = 2;
	int_map[5] = any_encode(temp_map);
	int_map[6] = any_encode(std::make_pair(temp_map, str_vec));
	int_map[7] = any_encode(std::make_tuple(std::string("string"), false, 1.0));
	std::vector<int> temp_int_vec = { 1, 2, 3, 4 };
	std::vector<float> temp_float_vec = { 1.0f, 2.0f, 3.0f, 4.0f };
	int_map[8] = any_encode(temp_int_vec);
	int_map[9] = any_encode(temp_float_vec);
	debug_show(int_map);
	std::cout << "result is:\n" <<encode(int_map) << std::endl;
	any_value_type decode_value;
	any_decode(any_encode(int_map), decode_value);
	std::cout << "after reencode result is:\n" << encode(decode_value) << std::endl;
	if (!any_decode(int_map[5], temp_map))
	{
		std::cout << "fail to decode 0" << encode(int_map[5]) << std::endl;
	}
	std::unordered_map<std::string, std::uint32_t> temp_map_1;
	if (!any_decode(int_map[5], temp_map_1))
	{
		std::cout << "fail to decode 1" << encode(int_map[5]) << std::endl;
	}
	std::unordered_map<std::string, std::uint16_t> temp_map_2;
	if (!any_decode(int_map[5], temp_map_2))
	{
		std::cout << "fail to decode 2" << encode(int_map[5]) << std::endl;
	}
	std::unordered_map<std::string, std::uint64_t> temp_map_3;
	if (!any_decode(int_map[5], temp_map_3))
	{
		std::cout << "fail to decode 3" << encode(int_map[5]) << std::endl;
	}
	std::unordered_map<std::string, std::int64_t> temp_map_4;
	if (!any_decode(int_map[5], temp_map_4))
	{
		std::cout << "fail to decode 4" << encode(int_map[5]) << std::endl;
	}
	
	if (!std::is_constructible_v<any_value_type, int>)
	{
		std::cout << "fail to construct any value from int" << std::endl;
	}
	if (!std::is_constructible_v<any_value_type, char>)
	{
		std::cout << "fail to construct any value from char" << std::endl;
	}
	if (!std::is_constructible_v<any_value_type, float>)
	{
		std::cout << "fail to construct any value from float" << std::endl;
	}
	if (!std::is_constructible_v<any_value_type, std::uint32_t>)
	{
		std::cout << "fail to construct any value from std::uint32_t" << std::endl;
	}
	if (!std::is_constructible_v<any_value_type, std::uint64_t>)
	{
		std::cout << "fail to construct any value from uint64" << std::endl;
	}

}

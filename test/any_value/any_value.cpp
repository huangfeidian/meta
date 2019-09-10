#include <meta/serialize/any_value.h>
#include <meta/serialize/decode.h>
#include <iostream>

using namespace std;
using namespace meta::serialize;
void debug_show(const any_value_type& cur_value)
{
	cout << "current any value is " << encode(cur_value) << endl;
	if (cur_value.is_int())
	{
		cout << "type is int" << endl;
	}
	if (cur_value.is_int64())
	{
		cout << "type is int64" << endl;
	}
	if (cur_value.is_float())
	{
		cout << "type is is_float" << endl;
	}
	if (cur_value.is_double())
	{
		cout << "type is is_double" << endl;
	}
	if (cur_value.is_bool())
	{
		cout << "type is is_bool" << endl;
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
	int_map[1] = "string";
	int_map[2] = 1.0;
	int_map[3] = false;
	std::vector<string> str_vec;
	str_vec.push_back("str1");
	str_vec.push_back("str2");
	int_map[4] = any_convert(str_vec);
	std::unordered_map<std::string, int> temp_map;
	temp_map["str1"] = 1;
	temp_map["str2"] = 2;
	int_map[5] = any_convert(temp_map);
	std::cout << "result is" <<encode(int_map) << std::endl;
	debug_show(int_map);

}

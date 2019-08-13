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
int main()
{
	container_size_print();
	container_encode_print();
}
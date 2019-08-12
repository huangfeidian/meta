#include <iostream>
#include <iomanip>
#include <serialize/decode.h>
#include <serialize/encode.h>
using namespace meta::serialize;
template <typename T>
void decode_test_print(const T& data)
{
	T dst;
	json encode_data = encode(data);
	std::cout << std::setw(4) << "encode result " << encode_data << std::endl;
	bool decode_result = decode(encode_data, dst);
	std::cout<<"decode result " <<  decode_result << " with encode result "<<json(encode(dst))<< std::endl;
}
void decode_test_primitive()
{
	int data_1 = 1;
	decode_test_print(data_1);
	float data_2 = 1.5f;
	decode_test_print(data_2);
	double data_3 = 2.0;
	decode_test_print(data_3);
	bool data_4 = true;
	decode_test_print(data_4);
	bool data_5 = false;
	decode_test_print(data_5);
	std::string data_6 = "hehhe";
	decode_test_print(data_6);
}

void decode_test_std_container()
{
	std::array<int, 5> data_1 = { 1,2,3,4,5 };
	decode_test_print(data_1);
	std::vector<int> data_2 = { 1,2,3,4,5 };
	decode_test_print(data_2);
	std::forward_list<int> data_3 = { 1,2,3,4,5 };
	decode_test_print(data_3);
	std::list<int> data_4 = { 1,2,3,4,5 };
	decode_test_print(data_4);

	std::set<int> data_5 = { 1,2,3,4,5 };
	decode_test_print(data_5);
	std::multiset<int> data_6 = { 1,2,3,4,5 };
	decode_test_print(data_6);
	std::unordered_set<int> data_7 = { 1,2,3,4,5 };
	decode_test_print(data_7);
	std::unordered_multiset<int> data_8 = { 1,2,3,4,5 };
	decode_test_print(data_8);

	std::pair<int, int> data_9 = { 10, 11 };
	decode_test_print(data_9);
	std::tuple<std::string, int, bool> data_10 = std::make_tuple("hehe", 1, false);
	decode_test_print(data_10);

	std::optional<std::string> data_11 = "hehe";
	decode_test_print(data_11);
	std::variant<std::string, bool, int> data_12 = std::string("fail");
	decode_test_print(data_12);
	std::optional<std::string> data_13;
	decode_test_print(data_13);
	std::variant<std::string, bool, int> data_14 ;
	decode_test_print(data_14);


	std::map<int, int> data_15 = { {1,2}, {3, 4}, {5, 6} };
	decode_test_print(data_15);
	std::multimap<int, int> data_16 = { {1,2}, {3, 4}, {5, 6} };
	decode_test_print(data_16);
	std::unordered_map<int, int> data_17 = { {1,2}, {3, 4}, {5, 6} };
	decode_test_print(data_17);
	std::unordered_multimap<int, int> data_18 = { {1,2}, {3, 4}, {5, 6} };
	decode_test_print(data_18);
}
struct c_1
{
	int a = 0;
	std::vector<int> b;
	std::array<std::string, 2> c = { "hehe", "haha" };
	json encode() const
	{
		json result = json::array();
		result.push_back(a);
		result.push_back(meta::serialize::encode(b));
		result.push_back(meta::serialize::encode(c));
		return result;
	}
	bool decode(const json& data)
	{
		if (!data.is_array())
		{
			return false;
		}
		if (data.size() != 3)
		{
			return false;
		}
		if (!meta::serialize::decode(data[0], a))
		{
			return false;
		}
		if (!meta::serialize::decode(data[1], b))
		{
			return false;
		}
		if (!meta::serialize::decode(data[2], c))
		{
			return false;
		}
		return true;
	}

};
struct c_2
{
	std::unordered_map<int, int> b;
	std::unordered_map<int, int> encode() const
	{
		return b;
	}
	bool decode(const std::unordered_map<int, int>& data)
	{
		b = data;
		return true;
	}
};
void decode_test_custom()
{
	c_1 data_1;
	decode_test_print(data_1);
	c_2 data_2;
	data_2.b[2] = 4;
	decode_test_print(data_2);
}
int main()
{
	decode_test_primitive();
	decode_test_std_container();
	decode_test_custom();
}
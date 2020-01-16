
#include <iostream>
#include <iomanip>

#include <cstdint>
#include <fstream>
#include <iomanip>

#include <iostream>
#include <queue>
#include <filesystem>

#include <meta/serialize/encode.h>

using namespace std;
using namespace spiritsaway::meta;
using namespace spiritsaway::meta::serialize;
template <typename T>
void print_encode_result(const T& data)
{
	json result = encode(data);
	cout << setw(4) << result << endl;
}
template <typename... Args>
void print_encode_multi_result(const Args&... args)
{
	json result = encode_multi(args...);
	cout << setw(4) << result << endl;
}
void encode_unit_test_data()
{
	print_encode_result(5);
	print_encode_result(0.5);
	print_encode_result(0.5f);
	print_encode_result(true);
	print_encode_result(false);
	print_encode_result("test");

	array<int, 5> data_1 = { 1, 2, 3, 4, 5 };
	print_encode_result(data_1);
	tuple<int, float> data_2 = make_tuple<int, float>(1, 1.0);
	print_encode_result(data_2);
	int data_3[5] = { 0 };
	print_encode_result(data_3);
	
	pair<int, float> data_4 = make_pair<int, float>(4, 0.5);
	print_encode_result(data_4);
	unordered_map<int, string> data_5 = { {1, "hehe"}, {2, "ahah"} };
	print_encode_result(data_5);
	unordered_set<int> data_6 = { 1,2,3,4 };
	print_encode_result(data_6);
	unordered_multimap<int, string> data_7 = { {1, "hehe"}, {1, "haha"} };
	print_encode_result(data_7);
	list<int> data_8 = { 1, 2, 3 };
	print_encode_result(data_8);
	vector<string> data_9 = { "e", "b" };
	print_encode_result(data_9);

	pair<string, string> data_10 = make_pair<string, string>("a,", "b");
	print_encode_result(data_10);

	vector<unordered_map<int, float>> data_11 = {{{1, 0.5}, {2, 0.3}}, {{4,0}}};
	print_encode_result(data_11);

	int a = 1;
	int b = 1;
	std::pair<int, int> data_12 = std::make_pair(a, b);
	print_encode_result(data_12);

	std::optional<int> data_13;
	print_encode_result(data_13);

	std::optional<int> data_14 = 5;
	print_encode_result(data_14);

	std::variant<int, std::string, bool> data_15;
	print_encode_result(data_15);

	std::variant<int, std::string, bool> data_16 = "hehhe";
	print_encode_result(data_16);

}
struct c_1
{
	int a = 1;
	int b = 2;
	std::pair<int, int> encode() const
	{
		return std::make_pair(a, b);
	}
};
void encode_object_test()
{


	c_1 data_1;
	print_encode_result(data_1);
	c_1 data_2;
	pair<c_1, c_1> data_3 = make_pair(data_1, data_2);
	print_encode_result(data_3);
	vector<c_1> data_4 = {data_1, data_2};
	print_encode_result(data_4);
	list<c_1> data_5 = {data_1, data_2};
	print_encode_result(data_5);

	unordered_map<int, c_1> data_6 = {{1, data_1}, {2, data_2}};
	print_encode_result(data_6);

}

void encode_multi_test()
{
	c_1 data_1;
	print_encode_multi_result(data_1);
	c_1 data_2;
	print_encode_multi_result(data_1, data_2);
	print_encode_multi_result(3);
	print_encode_multi_result("hehe");
	print_encode_multi_result(false);
	std::vector<int> data_3 = { 1, 2, 3 };
	std::vector<c_1> data_4 = { data_1, data_2};
	print_encode_multi_result(data_3);
	print_encode_multi_result(data_4);
	print_encode_multi_result(data_3, data_4);
}

int main()
{
	encode_unit_test_data();
	encode_object_test();
}
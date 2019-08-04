#include "encode.h"
#include <iostream>
#include <iomanip>
using namespace std;
template <typename T>
void print_encode_result(const T& data)
{
	json result = encode(data);
	cout << setw(4) << result << endl;
}
int main()
{
	array<int, 5> data_1 = { 1, 2, 3, 4, 5 };
	print_encode_result(data_1);
	tuple<int, float> data_2 = make_tuple<int, float>(1, 1.0);
	print_encode_result(data_2);
	
    
}
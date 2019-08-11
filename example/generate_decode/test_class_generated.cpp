#include "test_class.h"
json test::test_class::encode() const
{
	json result = json::array();
	//begin base encode
	result.push_back(encode(a);
	result.push_back(encode(b);
	result.push_back(encode(c);
	return result;
}

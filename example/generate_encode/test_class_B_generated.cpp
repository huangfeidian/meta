#include "test_class.h"
json test::test_class_B::encode() const
{
	json result = json::array();
//begin base encode
	result.push_back(encode(static_cast<const test::test_class&>(*this))
	result.push_back(encode(d);
	return result;
}

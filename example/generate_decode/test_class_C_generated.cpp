#include "test_class.h"
json test::test_class_C::encode() const
{
	json result = json::array();
	//begin base encode
	result.push_back(encode(f);
	return result;
}
bool test::test_class_C::decode(const json& data) 
{
	if(!data.is_array())
	{
		return false;
	}
	if(data.size() !=1)
	{
		return false;
	}
	//begin base decode
	//begin variable decode
	if(!decode(data[0], f)
	{
		return false;
	}
	return true;
}

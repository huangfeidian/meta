#include "test_class.h"
json test::test_class::encode() const
{
	json result = json::array();
	//begin base encode
	result.push_back(encode(a);
	result.push_back(encode(b);
	return result;
}
bool test::test_class::decode(const json& data) 
{
	if(!data.is_array())
	{
		return false;
	}
	if(data.size() !=2)
	{
		return false;
	}
	//begin base decode
	//begin variable decode
	if(!decode(data[0], a)
	{
		return false;
	}
	if(!decode(data[1], b)
	{
		return false;
	}
	return true;
}

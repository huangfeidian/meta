#include "test_property.h"
json test::test_property::encode() const
{
	json result = json::array();
	//begin base encode
	result.push_back(encode(a);
	result.push_back(encode(b);
	result.push_back(encode(c);
	result.push_back(encode(d);
	return result;
}
bool test::test_property::decode(const json& data) 
{
	if(!data.is_array())
	{
		return false;
	}
	if(data.size() !=4)
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
	if(!decode(data[2], c)
	{
		return false;
	}
	if(!decode(data[3], d)
	{
		return false;
	}
	return true;
}

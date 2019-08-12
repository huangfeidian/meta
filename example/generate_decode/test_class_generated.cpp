#include "test_class.h"
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

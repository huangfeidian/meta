#include "test_class.h"
bool test::test_class_B::decode(const json& data) 
{
	if(!data.is_array())
	{
		return false;
	}
	if(data.size() !=3)
	{
		return false;
	}
	//begin base decode
	if(!decode(data[0], static_cast<test::test_class&>(*this))
	{
		return false;
	}
	if(!decode(data[1], static_cast<test::test_class_C&>(*this))
	{
		return false;
	}
	//begin variable decode
	if(!decode(data[2], d)
	{
		return false;
	}
	return true;
}

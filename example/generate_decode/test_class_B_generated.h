public:
json encode() const
{
	json result = json::array();

	//begin base encode
	result.push_back(encode(static_cast<const test::test_class&>(*this)));

	//begin fields encode
	result.push_back(d);
	return result;
}
private:public:
bool decode(const json& data)
{
	if(!data.is_array())
	{
		return false;
	}
	if(data.size() != 2)
	{
		return false;
	}
	//begin base decode

	if(!decode(data[0], static_cast<test::test_class&>(*this)))
	{
		return false;
	}

	//begin field decode

	if(!decode(data[1], d))
	{
		return false;
	}
	return true;
}
private:
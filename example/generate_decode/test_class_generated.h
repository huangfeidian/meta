public:
json encode() const
{
	json result = json::array();

	//begin base encode
	

	//begin fields encode
	result.push_back(a);
	result.push_back(b);
	result.push_back(c);
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

	

	//begin field decode

	if(!decode(data[0], a))
	{
		return false;
	}
	if(!decode(data[1], b))
	{
		return false;
	}
	return true;
}
private:
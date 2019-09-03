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
private:
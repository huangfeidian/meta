public:
json encode() const
{
	json result = json::array();

	//begin base encode
	result.push_back(encode(static_cast<const test::test_class&>(*this)));
	result.push_back(encode(static_cast<const test::test_class_C&>(*this)));

	//begin fields encode
	
	return result;
}
private:
#include <serialize/container.h>
#include <iostream>
#include <iomanip>
#include <serialize/property.h>
using namespace std;
using namespace meta::serialize;
using namespace meta::utils;


class PropertyMap
{
public:
	PropertyMap(const PropertyMap* _in_parent, var_prefix_idx_type _in_cur_depth) :
		_parent(_in_parent),
		_cur_depth(_in_cur_depth)
	{

	}
	const PropertyMap* _parent;
	int a;
	std::vector<std::string> b;
	std::unordered_map<int, std::string> c;
	const decltype(a)& a_ref() const
	{
		return a;
	}
	prop_proxy<decltype(a)> a_mut()
	{
		return prop_proxy(a, _cmd_buffer, index_for_a, _cur_depth);
	}
	
	const decltype(b)& b_ref() const
	{
		return b;
	}
	prop_proxy<decltype(b)> b_mut()
	{
		return prop_proxy(b, _cmd_buffer, index_for_b, _cur_depth);
	}
	
	const decltype(c)& c_ref() const
	{
		return c;
	}
	prop_proxy<decltype(c)> c_mut()
	{
		return prop_proxy(c, _cmd_buffer, index_for_c, _cur_depth);
	}
	
	bool replay_mutate_msg(std::size_t field_index, var_mutate_cmd cmd, const json& data)
	{
		switch (field_index)
		{
		case index_for_a:
		{
			auto temp_proxy = a_mut();
			return temp_proxy.replay(cmd, data);
		}
		case index_for_b:
		{
			auto temp_proxy = b_mut();
			return temp_proxy.replay(cmd, data);
		}
		case index_for_c:
		{
			auto temp_proxy = c_mut();
			return temp_proxy.replay(cmd, data);
		}
		default:
			return false;
		}
	}
	bool operator==(const PropertyMap& other)
	{
		return a == other.a && b == other.b && c == other.c;
	}
	json encode() const
	{
		return encode_multi(a, b, c);
	}
private:
	const var_prefix_idx_type _cur_depth;
	const static var_idx_type index_for_a = 0;
	const static var_idx_type index_for_b = 1;
	const static var_idx_type index_for_c = 2;
public:
	std::deque<mutate_msg> _cmd_buffer;
};
void test_property_mutate()
{
	std::vector<std::uint8_t> depth = { 1,2,3 };
	PropertyMap test_a(nullptr, depth);
	PropertyMap test_b(nullptr, depth);
	mutate_msg msg;
	test_a.a_mut().set(1);
	msg = test_a._cmd_buffer.front();
	test_a._cmd_buffer.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	auto mut_b = test_a.b_mut();
	mut_b.set(std::vector<std::string>{"hehe", "hahah"});
	msg = test_a._cmd_buffer.front();
	test_a._cmd_buffer.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_b.push_back("ee");
	msg = test_a._cmd_buffer.front();
	test_a._cmd_buffer.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_b.pop_back();
	msg = test_a._cmd_buffer.front();
	test_a._cmd_buffer.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	auto mut_c = test_a.c_mut();
	mut_c.insert(1, "eh");
	msg = test_a._cmd_buffer.front();
	test_a._cmd_buffer.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_c.insert(1, "ahaha");
	msg = test_a._cmd_buffer.front();
	test_a._cmd_buffer.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_c.clear();
	msg = test_a._cmd_buffer.front();
	test_a._cmd_buffer.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_c.insert(3, "ahaha");
	msg = test_a._cmd_buffer.front();
	test_a._cmd_buffer.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_c.erase(3);
	msg = test_a._cmd_buffer.front();
	test_a._cmd_buffer.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	
	std::cout << "test a is " << test_a.encode() << std::endl;
	std::cout << "test b is " << test_b.encode() << std::endl;

}
int main()
{
	test_property_mutate();
}
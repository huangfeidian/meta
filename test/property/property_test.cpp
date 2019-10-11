
#include <iostream>
#include <iomanip>
#include <meta/serialize/property.h>
using namespace std;
using namespace meta::serialize;

class simple_item : public property_item<int>
{
public:
	int a = 0;
	using base = property_item<int>;
	using base::base;
	const static var_idx_type index_for_item = 0;
	const static var_idx_type index_for_a = 1;
	const decltype(a)& a_ref() const
	{
		return a;
	}
	prop_proxy<decltype(a)> a_mut()
	{
		return prop_proxy(a, _cmd_buffer, index_for_a);
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
		default:
			break;
		}
	}
	bool operator==(const simple_item& other)
	{
		return _id == other._id && a == other.a;
	}
	json encode() const
	{
		auto result = base::encode();
		result["a"] = a;
		return result;
	}
	bool decode(const json& data)
	{
		if (!base::decode(data))
		{
			return false;
		}
		auto iter = data.find("a");
		if (iter == data.end())
		{
			return false;
		}
		if (!::decode(*iter, a))
		{
			return false;
		}
		return true;
	}
};

class simple_bag : public property_bag<int, simple_item>
{
public:
	using base = property_bag<int, simple_item>;
	using base::base;
	const std::string& type_name() const
	{
		static std::string name = "simple_bag";
		return name;
	}
	bool replay_mutate_msg(std::size_t field_index,
		var_mutate_cmd cmd, const json& data)
	{
		switch (field_index)
		{
		case index_for_item:
			return true;
		default:
			break;
		}
		return true;
	}
	bool operator==(const simple_bag& other)
	{
		return base::operator==(other);
	}
	json encode() const
	{
		return base::encode();
	}
	bool decode(const json& data)
	{
		return base::decode(data);
	}
	const static var_idx_type index_for_item = 0;
};
class PropertyMap: public property_bag_base
{
public:
	PropertyMap(const PropertyMap* _in_parent, 
		var_prefix_idx_type _in_cur_depth, 
		std::deque<mutate_msg>& _mutate_queue) :
		property_bag_base(_cur_depth,_mutate_queue),
		_parent(_in_parent),
		_cur_depth(_in_cur_depth),
		_dest_buffer(_mutate_queue),
		_cmd_buffer(_dest_buffer, _cur_depth),
		g(concat_prefix(_cur_depth, index_for_g), _dest_buffer)
	{

	}
	
	int a;
	std::vector<std::string> b;
	std::unordered_map<int, std::string> c;
	any_vector d;
	any_int_map e;
	any_str_map f;
	simple_bag g;
	const std::string& type_name() const
	{
		static std::string name = "PropertyMap";
		return name;
	}
	const decltype(a)& a_ref() const
	{
		return a;
	}
	prop_proxy<decltype(a)> a_mut()
	{
		return prop_proxy(a, _cmd_buffer, index_for_a);
	}
	
	const decltype(b)& b_ref() const
	{
		return b;
	}
	prop_proxy<decltype(b)> b_mut()
	{
		return prop_proxy(b, _cmd_buffer, index_for_b);
	}
	
	const decltype(c)& c_ref() const
	{
		return c;
	}
	prop_proxy<decltype(c)> c_mut()
	{
		return prop_proxy(c, _cmd_buffer, index_for_c);
	}
	const decltype(d)& d_ref() const
	{
		return d;
	}
	prop_proxy<decltype(d)> d_mut()
	{
		return prop_proxy(d, _cmd_buffer, index_for_d);
	}
	const decltype(e)& e_ref() const
	{
		return e;
	}
	prop_proxy<decltype(e)> e_mut()
	{
		return prop_proxy(e, _cmd_buffer, index_for_e);
	}
	const decltype(f)& f_ref() const
	{
		return f;
	}
	prop_proxy<decltype(f)> f_mut()
	{
		return prop_proxy(f, _cmd_buffer, index_for_f);
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
		case index_for_d:
		{
			auto temp_proxy = d_mut();
			return temp_proxy.replay(cmd, data);
		}
		case index_for_e:
		{
			auto temp_proxy = e_mut();
			return temp_proxy.replay(cmd, data);
		}
		case index_for_f:
		{
			auto temp_proxy = f_mut();
			return temp_proxy.replay(cmd, data);
		}
		case index_for_g:
		{

		}
		default:
			return false;
		}
	}
	bool operator==(const PropertyMap& other)
	{
		return a == other.a && b == other.b &&
			c == other.c && d == other.d 
			&& f ==other.f && e == other.e
			&& g == other.g;
	}
	json encode() const
	{
		json result;
		result["a"] = ::encode(a);
		result["b"] = ::encode(b);
		result["c"] = ::encode(c);
		result["d"] = ::encode(d);
		result["e"] = ::encode(e);
		result["f"] = ::encode(f);
		result["g"] = ::encode(g);
		return result;
	}
	bool decode(const json& data)
	{

	}
private:
	const PropertyMap* _parent;
	const var_prefix_idx_type _cur_depth;
	const static var_idx_type index_for_item = 0;
	const static var_idx_type index_for_a = 1;
	const static var_idx_type index_for_b = 2;
	const static var_idx_type index_for_c = 3;
	const static var_idx_type index_for_d = 4;
	const static var_idx_type index_for_e = 5;
	const static var_idx_type index_for_f = 6;
	const static var_idx_type index_for_g = 7;
public:
	std::deque<mutate_msg>& _dest_buffer;
	msg_queue _cmd_buffer;
};

void test_property_mutate()
{
	std::deque<mutate_msg> msg_cmd_queue;
	std::vector<std::uint8_t> depth = { 1,2,3 };
	any_vector temp_any_vec;
	temp_any_vec.push_back(1);
	temp_any_vec.push_back("hehe"s);

	any_str_map temp_any_str_map;
	temp_any_str_map["hehe"] = 1;
	temp_any_str_map["haha"] = "what"s;
	temp_any_str_map["wtf"] = temp_any_vec;

	any_int_map temp_any_int_map;
	temp_any_int_map[1] = 2;
	temp_any_int_map[3] = "hahhah";
	temp_any_int_map[5] = temp_any_str_map;

	PropertyMap test_a(nullptr, depth, msg_cmd_queue);
	PropertyMap test_b(nullptr, depth, msg_cmd_queue);
	mutate_msg msg;
	test_a.a_mut().set(1);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	auto mut_b = test_a.b_mut();
	mut_b.set(std::vector<std::string>{"hehe", "hahah"});
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_b.push_back("ee");
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_b.pop_back();
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	auto mut_d = test_a.d_mut();

	mut_d.set(any_vector{"hehe"s, "hahah"s});
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	

	// mut_c begin
	auto mut_c = test_a.c_mut();
	mut_c.insert(1, "eh");
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_c.insert(1, "ahaha");
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_c.clear();
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_c.insert(3, "ahaha");
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_c.erase(3);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	// mutate d begin
	mut_d.push_back(any_encode("ee"));
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_d.push_back(any_encode(1));
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_d.push_back(temp_any_str_map);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}



	mut_d.pop_back();
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_d.pop_back();
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	// mutate e begin
	auto mut_e = test_a.e_mut();
	mut_e.insert(1, "eh");
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_e.insert(1, "ahaha");
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_e.clear();
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_e.insert(3, 4);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_e.erase(3);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	
	mut_e.insert(5, temp_any_int_map);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	// mutate f begin

	auto mut_f = test_a.f_mut();
	mut_f.insert("1", "eh");
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_f.insert("1", "ahaha");
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_f.clear();
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_f.insert("3", 4);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_f.erase("3");
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(std::get<1>(msg), std::get<2>(msg), std::get<3>(msg));
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_f.insert("5", temp_any_int_map);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
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
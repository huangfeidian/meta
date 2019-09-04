#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <iostream>
#include <utility/type_map.h>

using namespace std;
using namespace meta;
class test_a
{
    public:
    void func_1(int a)
    {
        std::cout<<"func_1 with a"<<a<<std::endl;
    }
    void func_2(int a, const std::string& b)
    {
		std::cout << "func_2" << " a " << a << " b" << b << std::endl;
    }
    void func_3(int a, std::string& c)
    {
        std::cout<<"func_3"<<" a "<<a<<" c" <<c<<std::endl;
    }
	void wrapper_for_func1(const std::vector<const void*>& data)
	{
		func_1(*reinterpret_cast<const int*>(data[0]));
	}
	void wrapper_for_func2(const std::vector<const void*>& data)
	{
		func_2(*reinterpret_cast<const int*>(data[0]), *reinterpret_cast<const std::string*>(data[1]));
	}
	void wrapper_for_func3(const std::vector<const void*>& data)
	{
		func_3(*reinterpret_cast<const int*>(data[0]), *const_cast<std::string*>(reinterpret_cast<const std::string*>(data[1])));
	}
    template <typename... Args>
    bool attr_call_func(const std::string& func_name, const Args&... args)
    {
		auto& cur_type_map = utils::type_map<std::string>();
		
        auto cur_iter = attr_func_map.find(func_name);
        if(cur_iter == attr_func_map.end())
        {
            return false;
        }

		auto cur_func_ptr = cur_iter->second.first;
		const auto& cur_func_require = cur_iter->second.second;

		if (cur_type_map.can_convert_to<Args...>(cur_func_require))
		{
			std::vector<const void*> arg_pointers;
			(arg_pointers.push_back(reinterpret_cast<const void*>(std::addressof(args))),...);
			(this->*cur_func_ptr)(arg_pointers);
			return true;
		}
		else
		{
			return false;
		}
        
    }
	static void register_types()
	{
		auto& cur_type_map = utils::type_map<std::string>();
		cur_type_map.register_type<int>("int");
		cur_type_map.register_type<int&>("int&");
		cur_type_map.register_type<const int&>("const int&");
		cur_type_map.register_type<std::string>("std::string");
		cur_type_map.register_type<std::string&>("std::string&");
		cur_type_map.register_type<const std::string&>("const std::string&");
	}
	using attr_func_type = void(test_a::*)(const std::vector<const void*>&);
	static unordered_map<std::string, std::pair<attr_func_type, std::vector<int>>> attr_func_map;
	static void register_attr_funcs()
	{
		auto func_1_arg_require = meta::utils::func_arg_type_ids<std::string, utils::function_arguments<decltype(&test_a::func_1)>::type>::result();
		auto func_2_arg_require = meta::utils::func_arg_type_ids<std::string, utils::function_arguments<decltype(&test_a::func_2)>::type>::result();
		auto func_3_arg_require = meta::utils::func_arg_type_ids<std::string, utils::function_arguments<decltype(&test_a::func_3)>::type>::result();
		attr_func_map["func_1"] = std::make_pair(&test_a::wrapper_for_func1, func_1_arg_require);
		attr_func_map["func_2"] = std::make_pair(&test_a::wrapper_for_func2, func_2_arg_require);
		attr_func_map["func_3"] = std::make_pair(&test_a::wrapper_for_func3, func_3_arg_require);
	}
public:
	std::string b;
	const std::string c;
	std::string& a;
	const std::string& d;
	test_a(std::string& f):
		b(f),
		c(f),
		d(f),
		a(f)
	{

	}
	using attr_var_type = std::size_t test_a::*;
	static unordered_map<std::string, std::pair<int, int>> attr_var_map;
	static void register_attr_vars()
	{
		auto& cur_type_map = utils::type_map<std::string>();
		attr_var_map["a"] = std::make_pair(0, cur_type_map.get_type_id<decltype(test_a::a)>());
		attr_var_map["b"] = std::make_pair(1, cur_type_map.get_type_id<decltype(test_a::b)>());
		attr_var_map["c"] = std::make_pair(2, cur_type_map.get_type_id<decltype(test_a::c)>());
		attr_var_map["d"] = std::make_pair(3, cur_type_map.get_type_id<decltype(test_a::d)>());

	}
	template <typename T>
	std::optional<std::reference_wrapper<const T>> attr_get_var(const std::string& var_name) const
	{
		auto& cur_type_map = utils::type_map<std::string>();
		auto[result_p, type_idx] = var_address_for_name(var_name);
		if (!result_p)
		{
			return {};
		}
		if (!cur_type_map.can_convert_from<const T&>(type_idx))
		{
			return {};
		}
		return std::cref(*reinterpret_cast<const T*>(result_p));
	}
private:
	std::pair<void*, int> var_address_for_name(const std::string& var_name) const
	{
		auto cur_iter = attr_var_map.find(var_name);
		if (cur_iter == attr_var_map.end())
		{
			return std::make_pair(nullptr, 0);
		}
		auto cur_type_idx = cur_iter->second.second;
		auto cur_var_idx = cur_iter->second.first;

		switch (cur_var_idx)
		{
		case 0:
		{
			void* p = const_cast<void*>(reinterpret_cast<const void*>(&a));
			return std::make_pair(p, cur_type_idx);
		}
		case 1:
		{
			void* p = const_cast<void*>(reinterpret_cast<const void*>(&b));
			return std::make_pair(p, cur_type_idx);
		}
		case 2:
		{
			void* p = const_cast<void*>(reinterpret_cast<const void*>(&c));
			return std::make_pair(p, cur_type_idx);
		}
		case 3:
		{
			void* p = const_cast<void*>(reinterpret_cast<const void*>(&d));
			return std::make_pair(p, cur_type_idx);
		}
		default:
			return std::make_pair(nullptr, 0);
			break;
		}
	}
public:
	template <typename T>
	std::optional<std::reference_wrapper<T>> attr_get_var_mut(const std::string& var_name)
	{
		auto& cur_type_map = utils::type_map<std::string>();
		auto[result_p, type_idx] = var_address_for_name(var_name);
		if (!result_p)
		{
			return {};
		}
		if (!cur_type_map.can_convert_from<T&>(type_idx))
		{
			return {};
		}
		return std::ref(*reinterpret_cast<T*>(result_p));
	}
};
template <typename T1, typename T2>
void convert_test_case()
{
	auto& cur_type_map = utils::type_map<std::string>();
	auto T1_name = cur_type_map.find<T1>();
	auto T2_name = cur_type_map.find<T2>();
	auto convert_result_1 = cur_type_map.can_convert_to<T1>(cur_type_map.get_type_id<T2>());
	std::cout << "try convert T1 " << T1_name.value() << " to T2 " << T2_name.value() << " with result " << convert_result_1 << std::endl;
	auto convert_result_2 = cur_type_map.can_convert_from<T1>(cur_type_map.get_type_id<T2>());
	std::cout << "try convert T1 " << T1_name.value() << " from T2 " << T2_name.value() << " with result " << convert_result_2 << std::endl;
}
template <typename T1>
void convert_test_multiple_cases()
{
	using t = T1;
	using c_t = const T1;
	using r_t = std::add_lvalue_reference_t<T1>;
	using cr_t = std::add_lvalue_reference_t<std::add_const_t<T1>>;
	convert_test_case<t, t>();
	convert_test_case<t, c_t>();
	convert_test_case<t, r_t>();
	convert_test_case<t, cr_t>();
	convert_test_case<c_t, t>();
	convert_test_case<c_t, c_t>();
	convert_test_case<c_t, r_t>();
	convert_test_case<c_t, cr_t>();
	convert_test_case<r_t, t>();
	convert_test_case<r_t, c_t>();
	convert_test_case<r_t, r_t>();
	convert_test_case<r_t, cr_t>();
	convert_test_case<cr_t, t>();
	convert_test_case<cr_t, c_t>();
	convert_test_case<cr_t, r_t>();
	convert_test_case<cr_t, cr_t>();
}
void convert_test()
{
	auto& cur_type_map = utils::type_map<std::string>();
	cur_type_map.register_type<int>("int");
	cur_type_map.register_type<int&>("int&");
	cur_type_map.register_type<const int&>("const int&");
	cur_type_map.register_type<const int>("const int");
	cur_type_map.register_type<std::string>("std::string");
	cur_type_map.register_type<std::string&>("std::string&");
	cur_type_map.register_type<const std::string&>("const std::string&");
	cur_type_map.register_type<const std::string>("const std::string");
	convert_test_multiple_cases<int>();
	convert_test_multiple_cases<std::string>();
	

}
decltype(test_a::attr_var_map) test_a::attr_var_map;
decltype(test_a::attr_func_map) test_a::attr_func_map;
int main()
{
	std::string hehe = "hehe";
	std::string& haha = hehe;
    test_a aa(hehe);
	aa.register_types();
	aa.register_attr_funcs();
	aa.register_attr_vars();
    aa.attr_call_func<int>(std::string("func_1"), 1);
    aa.attr_call_func("func_2", 1, haha);
    aa.attr_call_func("func_3", 1, haha);
	auto a_c_ref = aa.attr_get_var<std::string>("a");
	auto a_ref = aa.attr_get_var_mut<std::string>("a");
	auto d_c_ref = aa.attr_get_var<std::string>("d");
	auto d_ref = aa.attr_get_var_mut<std::string>("d");
	auto b_c_ref = aa.attr_get_var<std::string>("b");
	auto b_ref = aa.attr_get_var_mut<std::string>("b");
	auto c_c_ref = aa.attr_get_var<std::string>("c");
	auto c_ref = aa.attr_get_var_mut<std::string>("c");
	//convert_test();
}
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
        std::cout<<"func_1"<<std::endl;
    }
    void func_2(int a, const std::string& b)
    {
        std::cout<<"func_2"<<std::endl;
    }
    void func_3(int a, const std::string& c)
    {
        std::cout<<"func_3"<<std::endl;
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
		func_3(*reinterpret_cast<const int*>(data[0]), *reinterpret_cast<const std::string*>(data[1]));
	}
    template <typename... Args>
    bool call_by_name(const std::string& func_name, const Args&... args)
    {
		auto& cur_type_map = utils::type_map<std::string>();
		using func_type = void(test_a::*)(const std::vector<const void*>&);
		unordered_map<std::string, std::pair<func_type, std::vector<int>>> func_map; 
		auto func_1_arg_require = meta::utils::func_arg_type_ids<std::string, utils::function_arguments<decltype(&test_a::wrapper_for_func1)>::type>();
		auto func_2_arg_require = meta::utils::func_arg_type_ids<std::string, utils::function_arguments<decltype(&test_a::wrapper_for_func2)>::type>();
		auto func_3_arg_require = meta::utils::func_arg_type_ids<std::string, utils::function_arguments<decltype(&test_a::wrapper_for_func3)>::type>();
		func_map["func_1"] = std::make_pair(&test_a::wrapper_for_func1, func_1_arg_require);
		func_map["func_2"] = std::make_pair(&test_a::wrapper_for_func2, func_2_arg_require);
		func_map["func_3"] = std::make_pair(&test_a::wrapper_for_func3, func_3_arg_require);
        auto cur_iter = func_map.find(func_name);
        if(cur_iter==func_map.end())
        {
            return false;
        }

		auto cur_func_ptr = cur_iter->second.first;
		const auto& cur_func_require = cur_iter->second.second;
		if (cur_type_map.can_convert_to<Args...>(cur_func_require))
		{
			std::vector<const void*> arg_pointers;
			(arg_pointers.push_back(reinterpret_cast<const void*>(std::addressof(args))),...);
			*cur_func_ptr(arg_pointers);
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

};
int main()
{
    test_a aa;
	aa.register_types();
    aa.call_by_name<int>("func_1", 1);
    //aa.call_by_name("func_2", 1, "hehe");
    //aa.call_by_name("func_3", 1, "hehe");
}
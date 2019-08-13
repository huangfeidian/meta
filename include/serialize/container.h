#include "decode.h"
namespace meta::serialize
{
    using any_key_type = std::variant<std::string, int>;
    class any_value_type;
    class any_value_type: public std::variant<int, float, double, bool, std::string, std::vector<any_value_type>, std::unordered_map<any_key_type, any_value_type>> 
    {
        using base = std::variant<int, float, double, bool, std::string, std::vector<any_value_type>, std::unordered_map<any_key_type, any_value_type>>;
        using base::base;
        using base::operator=;
	};
    using any_list = std::vector<any_value_type>;
    using any_map = std::unordered_map<any_key_type, any_value_type>;
    template <std::size_t N>
    using any_array = std::array<any_value_type, N>;
    using any_int_map = std::unordered_map<int, any_value_type>;
    using any_str_map = std::unordered_map<std::string, any_value_type>;

}


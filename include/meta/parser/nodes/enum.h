#pragma once
#include "type_info.h"
#include "forward.h"


namespace meta::language
{
    class enum_node: public node_base
    {
    public:
        enum_node(const node* _in_node);
    private:
        std::vector<std::string> _keys;
        std::vector<std::uint64_t> _values;
		std::uint8_t _int_width;//8 for std::uint8_t 16 for std::uint16_t 32 for std::uint32_t 64 for std::uint64_t
		void parse();
    public:
        const std::vector<std::string>& keys() const;
        const std::vector<std::uint64_t>& values() const;
        std::uint64_t value_for(const std::string& _in_key) const;
        std::uint8_t value_int_width() const;
		json to_json() const;


	};
}
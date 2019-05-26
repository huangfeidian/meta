#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <vector>


#include "../node.h"


namespace meta::language
{
    class enum_node: public node_base
    {
    public:
        enum_node(const node* _in_node);
    private:
        std::vector<std::string> _keys;
        std::vector<std::uint32_t> _values;
        std::string _value_type_name; // std::uint32_t std::uint8_t
    public:
        const std::vector<std::string>& keys() const;
        const std::vector<std::uint32_t>& values() const;
        std::uint32_t value_for(const std::string& _in_key) const;
        const std::string& value_type() const;

    }
}
#pragma once

#include "node_base.h"
#include "../name_space.h"
namespace meta::language
{
    class type_info: public node_base
    {
    public:
        
        const node* base_type() const; // return the base type example: int vector
        const std::vector<const type_info*>& depend_args() const;
        bool is_completed() const; // is this a type that depends on other variable
        bool is_templated() const;// is this a template type
    private:
        type_info(const node* _in_node);
        std::vector<const type_info*> _args;
        friend class type_db;
    }
    class type_db
    {
    public:
        const type_info* get_type(const node* _in_node);
        static type_db& instance()
        {
            static type_db _instance;
            return type_db;
        }
        void clear_all_type_info();
    private:
        std::unordered_map<std::string, type_info*> _type_data; // qualitified name to type_info
        type_db();

    }
}
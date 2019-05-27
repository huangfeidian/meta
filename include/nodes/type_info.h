#pragma once

#include "node_base.h"
#include "../name_space.h"
namespace meta::language
{
    class type_info
    {
    public:
        
        const node* base_type() const; // return the base type example: int vector
        const std::vector<const type_info*>& depend_args() const;
        bool is_completed() const; // is this a type that depends on other variable
        bool is_templated() const;// is this a template type
		bool is_base() const; // is this a type declare
		bool is_callable() const; // is this a callable
    private:
        type_info(const CXType* _in_type);
        std::vector<const type_info*> _args;
		const CXType* _type;
		const CXType* _base_type;
        friend class type_db;
	};
    class type_db
    {
    public:
        const type_info* get_type(const CXType* _in_type);
        static type_db& instance()
        {
            static type_db _instance;
            return _instance;
        }
        void clear_all_type_info();
    private:
        std::unordered_map<std::string, type_info*> _type_data; // qualitified name to type_info
        type_db();

	};
}
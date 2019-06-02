#pragma once

#include "node_base.h"
#include "../name_space.h"
namespace meta::language
{
    class type_info
    {
    public:
        
        const node* base_type() const; // return the base type example: int vector
        const std::vector<const type_info*>& template_args() const;
        bool is_completed() const; // is this a type that depends on other variable not in current lexical scope
        bool is_templated() const;// is this a template type
		bool is_base() const; // is this a type declare
		bool is_callable() const; // is this a callable
        bool is_const() const;// is const
        bool is_volatile() const;// is volatile
        bool is_reference() const; // is reference
		bool is_lvalue_refer() const;// is &
		bool is_rvalue_refer() const;// is &&
		const std::string& qualified_name() const;
		const std::string& name() const;

    private:
        type_info(const CXType _in_type);
        std::vector<const type_info*> _template_args;
		CXType _type;
		type_info* _unqualified_type;
        friend class type_db;
	};
    class type_db
    {
    public:
        const type_info* get_type(const CXType* _in_type);
        const type_info* get_type(CXCursor in_cursor);
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
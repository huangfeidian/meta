#pragma once

#include <nlohmann/json.hpp>
#include "node_base.h"
#include "../name_space.h"
namespace meta::language
{
	using json = nlohmann::json;
    class type_info
    {
    public:
        
        const type_info* ref_type() const; // return the base type example: int vector
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
		bool is_pointer() const; // is T* const T*
		const type_info* point_to() const;// return T for T * return const T for const T * else nullptr
		const type_info* refer_to() const;// return (const) T for (const) T &/&& return nullptr for others
		enum CXTypeKind kind() const;
		const CXType& type() const;
		const std::string& name() const;
		bool has_type() const;
		json to_json() const;
    private:

		void set_type(CXType _in_type);
        type_info(const std::string& _in_name, const CXType& _in_type, const type_info* _in_ref_type);
		type_info(const std::string& _in_name, enum CXTypeKind _in_kind);
        std::vector<const type_info*> _template_args;
		CXType _type;
		const std::string _name;
		enum CXTypeKind _kind;
		const type_info* _ref_type;
        friend class type_db;
	};
    class type_db
    {
    public:
		type_info* get_type(const CXType& _in_type);
        static type_db& instance()
        {
            static type_db _instance;
            return _instance;
        }
        void clear_all_type_info();
		json to_json() const;
    private:
		type_info* get_type_for_const(const CXType& _in_type);
		type_info* get_type_for_pointee(const CXType& _in_type);
		type_info* get_type_for_template(const CXType& _in_type);
        std::unordered_map<std::string, type_info*> _type_data; // qualitified name to type_info
        type_db();

	};
}
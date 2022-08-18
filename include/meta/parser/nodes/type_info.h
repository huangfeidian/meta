#pragma once

#include <nlohmann/json.hpp>
#include "node_base.h"
#include "../name_space.h"
#include "forward.h"
using json = nlohmann::json;
namespace spiritsaway::meta::language
{
	class type_info
	{
	public:
		
		const type_info* ref_type() const; // return the base type example: int vector
		const std::vector<const type_info*>& template_args() const;
		bool is_alias() const;
		bool is_completed() const; // is this a type that depends on other variable not in current lexical scope
		bool is_templated() const;// is this a template type
		bool is_template_arg() const; // is this a template arg type for example typename T
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
		const type_info* alias_to() const; // return typedef or alias type_info if any
		enum CXTypeKind kind() const;
		CXType type() const;
		const std::string& name() const;
		const std::string& unqualified_name() const;
		const std::string& qualified_name() const;
		const std::string& pretty_name() const;
		bool has_type() const;
		const class_node* related_class() const;
		bool set_related_class(class_node* _in_class);
		bool can_accept_arg_type(const type_info* arg_type) const;
		json to_json() const;
	private:

		void set_type(CXType _in_type);
		type_info(const std::string& _in_name, CXType _in_type, const type_info* _in_ref_type);
		type_info(const std::string& _in_name, enum CXTypeKind _in_kind);
		std::vector<const type_info*> m_template_args;
		CXType m_type;
		const std::string m_name;
		mutable std::string m_pretty_name;
		enum CXTypeKind m_kind;
		const type_info* m_ref_type;
		class_node* m_related_class = nullptr;
		friend class type_db;

	};
	class type_db
	{
	public:
		type_info* get_type(CXType _in_type);
		class_node* get_class(const std::string& _class_name);
		bool add_class(class_node* _cur_class);
		bool add_enum(enum_node* _cur_enum);
		type_info* get_alias_typedef(CXCursor _in_cursor);
		type_info* get_type_for_template_class(CXCursor _template_class_decl);
		static type_db& instance()
		{
			static type_db _instance;
			return _instance;
		}
		void clear_all_type_info();
		json to_json() const;
		template <typename T>
		std::vector<const class_node*> get_class_with_pred(T _pred) const;
		template <typename T>
		std::vector<const enum_node*> get_enum_with_pred(T _pred) const;
		void build_class_under_namespace(const std::string& _ns_name);
		void create_from_translate_unit(CXCursor _tu_cursor);
		void add_alternate_name(CXType _in_type, const std::string& annotated_typename);
		const std::vector<std::pair<std::string, std::string>>& annotated_typenames() const
		{
			return m_annotated_typenames;
		}
	private:
		type_info* get_type_for_const(CXType _in_type);
		type_info* get_type_for_pointee(CXType _in_type);
		type_info* get_type_for_template(CXType _in_type);
		
		type_info* get_base_type_for_template(CXType _in_type);

	private:
		// 原始名字越长的放在前面
		std::vector<std::pair<std::string, std::string>> m_annotated_typenames; // from long name to short name
		std::unordered_map<std::string, type_info*> _type_data; // qualified name to type_info
		std::unordered_map<std::string, class_node*> _class_data;// qualified name to class_node
		std::unordered_map<std::string, enum_node*> _enum_data;// qualified name to enum class
		type_db();

	};
	template <typename T>
	std::vector<const class_node*> type_db::get_class_with_pred(T _pred) const
	{
		std::vector<const class_node*> result;
		for (const auto& i : _class_data)
		{
			if (!i.second)
			{
				continue;
			}
			if (_pred(*i.second))
			{
				result.push_back(i.second);
			}
		}
		return result;
	}
	template <typename T>
	std::vector<const enum_node*> type_db::get_enum_with_pred(T _pred) const
	{
		std::vector<const enum_node*> result;
		for (const auto& i : _enum_data)
		{
			if (!i.second)
			{
				continue;
			}
			if (_pred(*i.second))
			{
				result.push_back(i.second);
			}
		}
		return result;
	}
}
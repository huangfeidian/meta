#pragma once
#include <vector>
#include <unordered_map>
namespace meta
{

	using T_args = std::vector<std::uint32_t>;
	class basic_type_info
	{
		// for simple type and template type
	public:
		const std::uint32_t _id; // invalid id == 0
		const std::string _name;
		const std::uint32_t _ns_id;
		const std::uint32_t _template_id; // if this type is template _id == _template_id
		const T_args _template_args; // the non instantiated args is 0
		bool is_valid() const;
		std::string get_qualified_name() const;
		bool is_template() const;
		bool is_completed() const; // is this type can
	};
	class type_db
	{
	private:
		type_db();
		std::unordered_map<std::uint32_t, basic_type_info> _all_types; // from type id to type
		std::unordered_map<std::string, std::uint32_t> _qualified_name_to_type; // from qualified type name to type just for not template type and template decl
		std::uint32_t _type_id_counter;
	public:

		const basic_type_info& create_simple_type(std::uint32_t in_ns_id, const std::string& in_name);
		const basic_type_info& create_template_decl(std::uint32_t in_ns_id, const std::string& in_name, std::uint32_t args_size);
		const basic_type_info& create_template_instance(std::uint32_t in_ns_id, std::uint32_t in_template_id, const std::vector<std::uint32_t>& in_template_args);
		const basic_type_info& get_type_info(std::uint32_t in_type_id);
		static type_db& get_instance();
	};
}
#pragma once
#include <vector>
#include <unordered_map>
namespace meta
{
	class name_space
	{
	public:

		std::string get_full_name() const;
		bool is_anonymous() const;
		bool is_alias() const;
	private:
		name_space();
		name_space(std::uint32_t in_id, const std::string& in_name, std::uint32_t in_alias_id, std::uint32_t in_upper_ns);
		name_space& operator=(const name_space& other);
		name_space(const name_space& other);
		std::uint32_t _id;
		std::string _name;
		std::uint32_t _alias_id; // if the namespace is not alias _alias == _id
		std::uint32_t _upper_ns;
		friend class name_space_db;
		friend void swap(name_space& a , name_space& b);
	};
	class name_space_db
	{
	private:
		name_space_db();
		name_space_db(const name_space_db& other) = delete;
		std::unordered_map<std::uint32_t, name_space> all_namespaces;
		std::unordered_map<std::uint32_t, std::uint32_t> _upper_ns;
		std::unordered_map<std::uint32_t, std::vector<std::uint32_t>> _nest_ns;
		std::uint32_t _name_space_counter;
	public:
		const name_space& get_name_space(std::uint32_t in_ns_id) const;
		const name_space& create_name_space(const std::string& in_name, std::uint32_t in_upper_ns);
		const name_space& create_alias_name_space(const std::string& in_name, std::uint32_t in_upper_ns, std::uint32_t in_alias_ns);
		static name_space_db& get_instance();
		const static std::uint32_t global_ns_id = 0;
		const static std::uint32_t invalid_ns_id = 1;
	};
}
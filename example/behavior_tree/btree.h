#include <vector>
#include <meta/serialize/any_value.h>
namespace behavior
{
	class node_desc
	{
	public:
		std::string _type;
		std::vector<std::uint16_t> _sub_nodes;
		meta::serialize::any_str_map _desc;
		std::uint16_t _index;
	};
	class btree
	{
	public:
		btree(const std::string& btree_file_path)
		{

		}
	public:
		std::vector<meta::serialize::any_str_map> nodes;
		std::string tree_name;
		std::string signature;
	};

}
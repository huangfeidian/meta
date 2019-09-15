#include <vector>
#include <meta/serialize/any_value.h>
namespace behavior
{
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
#include <vector>
#include <meta/serialize/any_value.h>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <meta/utility/logger.h>
namespace behavior
{
	using node_idx_type = std::uint32_t;
	class node_desc
	{
	public:
		node_idx_type idx;
		std::string type;
		std::vector<node_idx_type> children;
		node_idx_type parent_idx;
		meta::serialize::any_str_map extra;
		node_desc(const meta::serialize::any_str_map &data)
		{
			auto idx_iter = data.find("idx");
			if (idx_iter == data.end())
			{
				return;
			}
			if (!idx_iter->second.is_int())
			{
				return;
			}
			idx = std::get<int>(idx_iter->second);

			auto parent_idx_iter = data.find("parent_idx_iter");
			if (parent_idx_iter == data.end())
			{
				return;
			}
			if (!parent_idx_iter->second.is_int())
			{
				return;
			}
			parent_idx = std::get<int>(parent_idx_iter->second);

			auto children_iter = data.find("children");
			if (children_iter == data.end())
			{
				return;
			}
			if (!children_iter->second.is_vector())
			{
				return;
			}
			for (auto& one_child : std::get<meta::serialize::any_vector>(children_iter->second))
			{
				if (!one_child.is_int())
				{
					return;
				}
				children.push_back(std::get<int>(one_child));
			}
			auto extra_iter = data.find("extra");
			if (extra_iter == data.end())
			{
				return;
			}
			if (!extra_iter->second.is_str_map())
			{
				return;
			}
			extra = std::get<meta::serialize::any_str_map>(extra_iter->second);

			auto type_iter = data.find("type");
			if (type_iter == data.end())
			{
				return;
			}
			if (!type_iter->second.is_str())
			{
				return;
			}
			type = std::get<std::string>(type_iter->second);

		}
		bool is_valid() const
		{
			return !type.empty();
		}
	};
	class btree_desc
	{
	private:
		bool load_from_json(const json& btree_file_data)
		{
			meta::serialize::any_value_type parse_result = meta::serialize::any_encode(btree_file_data);
			if (!parse_result.is_str_map())
			{
				return false;
			}
			const auto& cur_map = std::get<meta::serialize::any_str_map>(parse_result);

			auto tree_name_iter = cur_map.find("name");
			if (tree_name_iter == cur_map.end())
			{
				return false;
			}
			if (tree_name_iter->second.is_str())
			{
				return false;
			}
			tree_name = std::get<std::string>(tree_name_iter->second);

			auto extra_iter = cur_map.find("extra");
			if (extra_iter == cur_map.end())
			{
				return false;
			}
			if (extra_iter->second.is_str_map())
			{
				return false;
			}
			extra = std::get<meta::serialize::any_str_map>(extra_iter->second);

			auto nodes_iter = cur_map.find("nodes");
			if (nodes_iter == cur_map.end())
			{
				return false;
			}
			auto& cur_nodes = nodes_iter->second;
			if (!cur_nodes.is_vector())
			{
				return false;
			}

			for (auto& one_node : std::get<meta::serialize::any_vector>(cur_nodes))
			{
				if (!one_node.is_str_map())
				{
					return false;
				}
				auto new_node = node_desc(std::get<meta::serialize::any_str_map>(one_node));
				if (!new_node.is_valid())
				{
					return false;
				}
				if (nodes.size() != new_node.idx)
				{
					return false;
				}
				nodes.push_back(new_node);
			}


		}
	public:
		btree_desc(const std::string& btree_file_path)
		{
			auto btree_file_stream = std::ifstream(btree_file_path);
			std::string btree_file_str = std::string(std::istreambuf_iterator<char>(btree_file_stream), 
				std::istreambuf_iterator<char>());
			json data = json::parse(btree_file_str, nullptr, false);
			

			if (!load_from_json(data))
			{
				nodes.clear();
				return;
			}
		}
	public:
		std::vector<node_desc> nodes;
		std::string tree_name;
		std::string signature;
		meta::serialize::any_str_map extra;
		
		bool is_valid() const
		{
			return !nodes.empty();
		}
		static std::unordered_map<std::string, const btree_desc*> tree_cache;
		static const btree_desc* load(const std::string& file_path)
		{
			auto cur_iter = tree_cache.find(file_path);
			if (cur_iter != tree_cache.end())
			{
				return cur_iter->second;
			}
			auto new_btree = new btree_desc(file_path);
			tree_cache[file_path] = new_btree;
			return new_btree;
		}
	};

}
#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <clang-c/Index.h>
#include <utility>
#include <queue>
#include <meta/parser/clang_utils.h>

namespace meta::language
{
	enum class node_visit_result
	{
		visit_break,//exit the visit and return
		visit_continue, // not visit the children , visit next sibling
		visit_recurse// visit the children

	};
	class node
	{
	public:
		std::string name;	
		std::string comment;
		const std::string& qualified_name;
		node* _parent;
		CXCursorKind _kind;
		CXCursor _cursor; 
		std::vector<node*> _children;
		std::vector<node*> get_children_with_kind(CXCursorKind _filter_kind) const;
		const std::vector<node*>& get_all_children() const;
		const std::string& get_qualified_name() const;
		const std::string& get_brief_name() const;
		CXCursor get_cursor() const;
		CXCursorKind get_kind() const;
		const std::string& get_name() const;
		std::tuple<std::string, unsigned, unsigned> get_position() const;
		node& get_parent() const;
		friend class node_db;
	private:
		node(CXCursor in_cursor);
		void init_children();
		

	};

	class node_db
	{
	private:
		node_db();
	protected:
		std::unordered_map<CXCursor, node*, meta::utils::cursor_hash, meta::utils::cursor_equal> _nodes;
	public:
		node* create_node(CXCursor _cursor);
		static node_db& get_instance();
	};

	template <typename T>
	void bfs_visit_nodes(const node* head, T& visitor)
	{
		std::queue<const node*> _nodes_to_visit;// bool for first visit
		_nodes_to_visit.push(head);
		while (!_nodes_to_visit.empty())
		{
			auto current_node = _nodes_to_visit.front();
			_nodes_to_visit.pop();
			node_visit_result visit_result = visitor(current_node);
			switch (visit_result)
			{
			case node_visit_result::visit_break:
				return;
			case node_visit_result::visit_continue:
				break;
			case node_visit_result::visit_recurse:
				for (const auto& i : current_node->get_all_children())
				{
					_nodes_to_visit.push(i);
				}
				break;
			default:
				break;
			}

		}
	}
}
#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <clang-c/Index.h>


namespace meta::language
{
	class node
	{
	public:
		std::string name;	
		std::string qualified_name;
		node* _parent;
		CXTypeKind _kind;
		CXCursor _cursor; 
		std::vector<node*> _children;
		std::vector<node*> get_children_with_kind(CXTypeKind _filter_kind) const;
		const std::vector<node*> get_all_children() const;
		const std::string& get_qualified_name() const;
		CXCursor get_cursor() const;
		CXTypeKind get_kind() const;
		const std::string& get_name();
		node& get_parent() const;

	};

	class node_db
	{
	private:
		node_db();
	protected:
		std::unordered_map<std::string, node*> _nodes;
	public:
		node* create_node(CXCursor _cursor);
		static node_db& get_instance();
	};
}
#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <clang-c/Index.h>
#include <utility>

namespace meta::language
{
	class node
	{
	public:
		std::string name;	
		const std::string& qualified_name;
		node* _parent;
		CXCursorKind _kind;
		CXCursor _cursor; 
		std::vector<node*> _children;
		std::vector<node*> get_children_with_kind(CXCursorKind _filter_kind) const;
		const std::vector<node*>& get_all_children() const;
		const std::string& get_qualified_name() const;
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
	class cursor_hash
	{
	public:
		std::size_t operator()(const CXCursor& in_cursor) const
		{
			return clang_hashCursor(in_cursor);
		}

	};
	class cursor_equal
	{
	public:
		bool operator()(const CXCursor& from, const CXCursor& to) const
		{
			return clang_equalCursors(from, to);
		}
	};
	class node_db
	{
	private:
		node_db();
	protected:
		std::unordered_map<CXCursor, node*, cursor_hash, cursor_equal> _nodes;
	public:
		node* create_node(CXCursor _cursor);
		static node_db& get_instance();
	};
}
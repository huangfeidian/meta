#include <algorithm>
#include "node.h"
namespace 
{
	std::string to_string(const CXString &str)
	{
		auto temp = clang_getCString(str);
		std::string result(temp);
		clang_disposeString(str);
	}
}

namespace meta::language
{
	CXCursor node::get_cursor() const
	{
		return _cursor;
	}
	CXTypeKind node::get_kind() const
	{
		return _kind;
	}
	const std::string& node::get_name() const
	{
		return name;
	}
	const std::string& node::get_qualified_name() const
	{
		return qualified_name;
	}
	node& node::get_parent() const
	{
		return *_parent;
	}
	const std::vector<node*>& node::get_all_children() const
	{
		return _children;
	}
	std::vector<node*> node::get_children_with_kind(CXTypeKind _filter_kind) const
	{
		std::vector<node*> result;
		std::copy_if(_children.cbegin(), _children.cend(), std::back_inserter(result), [_filter_kind](const node* cur_node){
			return cur_node && cur_node->get_kind() == _filter_kind;
		})
		return result;
	}
	node_db& node_db::get_instance()
	{
		static node_db _instance;
		return _instance;
	}
	node* node_db::create_node(CXCursor _cursor)
	{

	}
}
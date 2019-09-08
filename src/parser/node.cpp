#include <algorithm>
#include <meta/parser/node.h>
#include <meta/parser/clang_utils.h>
#include <meta/parser/name_space.h>

namespace 
{
	
	using namespace meta::language;

}

namespace meta::language
{

	node::node(CXCursor in_cursor)
		: _cursor(in_cursor)
		, _kind(in_cursor.kind)
		, name(meta::utils::to_string(clang_getCursorDisplayName(in_cursor)))
		, qualified_name(utils::full_name(in_cursor))
	{
		if (in_cursor.kind == CXCursorKind::CXCursor_TranslationUnit)
		{
			_parent = nullptr;
		}
		else
		{
			_parent = node_db::get_instance().create_node(clang_getCursorSemanticParent(in_cursor));
			_parent->_children.push_back(this);
		}
	}
	CXCursor node::get_cursor() const
	{
		return _cursor;
	}
	CXCursorKind node::get_kind() const
	{
		return _kind;
	}
	const std::string& node::get_name() const
	{
		return qualified_name;
	}
	const std::string& node::get_brief_name() const
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
	std::vector<node*> node::get_children_with_kind(CXCursorKind _filter_kind) const
	{
		std::vector<node*> result;
		std::copy_if(_children.cbegin(), _children.cend(), std::back_inserter(result), [_filter_kind](const node* cur_node){
			return cur_node && cur_node->get_kind() == _filter_kind;
		});
		return result;
	}
	std::tuple<std::string, unsigned, unsigned> node::get_position() const
	{
		auto range = clang_Cursor_getSpellingNameRange(_cursor, 0, 0);

		auto start = clang_getRangeStart(range);

		CXFile file;
		unsigned line, column, offset;

		clang_getFileLocation(start, &file, &line, &column, &offset);
		std::string filename = utils::to_string(clang_getFileName(file));
		return std::make_tuple(filename, line, column);

	}
	node_db& node_db::get_instance()
	{
		static node_db _instance;
		return _instance;
	}
	node* node_db::create_node(CXCursor _cursor)
	{

		auto cur_iter = _nodes.find(_cursor);
		if (cur_iter != _nodes.end())
		{
			return cur_iter->second;
		}
		auto temp_node = new node(_cursor);
		_nodes[_cursor] = temp_node;
		auto cursor_pos = temp_node->get_position();
		utils::get_logger().debug("new node name {0} qualified name {1} kind {2}::{6} at file {3} row {4} col {5}", 
			temp_node->get_name(), temp_node->get_qualified_name(), _cursor.kind, std::get<0>(cursor_pos), std::get<1>(cursor_pos), std::get<2>(cursor_pos), utils::to_string(_cursor.kind));
		if (temp_node->get_kind() == CXCursor_Namespace)
		{
			name_space::get_name_space_for_node(temp_node);
		}
		return temp_node;
	}
	node_db::node_db()
	{

	}
}
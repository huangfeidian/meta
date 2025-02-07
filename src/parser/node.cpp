#include <algorithm>
#include <meta/parser/node.h>
#include <meta/parser/clang_utils.h>
#include <meta/parser/name_space.h>

namespace 
{
	
	using namespace spiritsaway::meta::language;

}

namespace spiritsaway::meta::language
{

	node::node(CXCursor in_cursor)
		: m_cursor(in_cursor)
		, m_kind(in_cursor.kind)
		, name(meta::utils::to_string(clang_getCursorDisplayName(in_cursor)))
		, qualified_name(utils::full_name(in_cursor))
	{
		comment = utils::to_string(clang_Cursor_getRawCommentText(in_cursor));
		if (in_cursor.kind == CXCursorKind::CXCursor_TranslationUnit)
		{
			m_parent = nullptr;
		}
		else
		{
			m_parent = node_db::get_instance().create_node(clang_getCursorSemanticParent(in_cursor));
			m_parent->m_children.push_back(this);
		}
	}
	CXCursor node::get_cursor() const
	{
		return m_cursor;
	}
	CXCursorKind node::get_kind() const
	{
		return m_kind;
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
		return *m_parent;
	}
	const std::vector<node*>& node::get_all_children() const
	{
		return m_children;
	}
	std::vector<node*> node::get_children_with_kind(CXCursorKind _filter_kind) const
	{
		std::vector<node*> result;
		std::copy_if(m_children.cbegin(), m_children.cend(), std::back_inserter(result), [_filter_kind](const node* cur_node){
			return cur_node && cur_node->get_kind() == _filter_kind;
		});
		return result;
	}
	std::tuple<std::string, unsigned, unsigned> node::get_position() const
	{
		auto range = clang_Cursor_getSpellingNameRange(m_cursor, 0, 0);

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
	node* node_db::create_node(CXCursor m_cursor)
	{

		auto cur_iter = _nodes.find(m_cursor);
		if (cur_iter != _nodes.end())
		{
			return cur_iter->second;
		}
		auto temp_node = new node(m_cursor);
		_nodes[m_cursor] = temp_node;
		auto cursor_pos = temp_node->get_position();
		utils::get_logger().debug("new node name {0} qualified name {1} kind {2}::{6} at file {3} row {4} col {5}", 
			temp_node->get_name(), temp_node->get_qualified_name(), int(m_cursor.kind), std::get<0>(cursor_pos), std::get<1>(cursor_pos), std::get<2>(cursor_pos), utils::to_string(m_cursor.kind));
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
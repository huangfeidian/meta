#include <algorithm>
#include "node.h"
#include "utils.h"

namespace 
{
	
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
	const std::string& get_qualified_name_from_cursor(CXCursor in_cursor)
	{
		static std::unordered_map< CXCursor, std::string, cursor_hash, cursor_equal> qualified_cache;
		auto cur_iter = qualified_cache.find(in_cursor);
		if (cur_iter != qualified_cache.end())
		{
			return cur_iter->second;
		}
		std::string result;
		if (in_cursor.kind == CXCursorKind::CXCursor_TranslationUnit)
		{
			qualified_cache[in_cursor] = "";
		}
		else
		{
			auto parent = clang_getCursorSemanticParent(in_cursor);
			const std::string& parent_name = get_qualified_name_from_cursor(parent);
			if (parent_name.empty())
			{
				qualified_cache[in_cursor] = meta::utils::to_string(clang_getCursorDisplayName(in_cursor));
			}
			else
			{
				qualified_cache[in_cursor] = get_qualified_name_from_cursor(parent) + "::" + meta::utils::to_string(clang_getCursorDisplayName(in_cursor));
			}
			
		}
		return qualified_cache[in_cursor];

	}
}

namespace meta::language
{

	node::node(CXCursor in_cursor)
		: _cursor(in_cursor)
		, _kind(in_cursor.kind)
		, name(meta::utils::to_string(clang_getCursorDisplayName(in_cursor)))
		, qualified_name(get_qualified_name_from_cursor(in_cursor))
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
		auto& cur_name = get_qualified_name_from_cursor(_cursor);
		auto cur_iter = _nodes.find(cur_name);
		if (cur_iter != _nodes.end())
		{
			return cur_iter->second;
		}
		auto temp_node = new node(_cursor);
		_nodes[temp_node->get_qualified_name()] = temp_node;
		auto cursor_pos = temp_node->get_position();
		utils::get_logger().debug("new node name {0} qualified name {1} kind {2} at file {3} row {4} col {5}", meta::utils::to_string(clang_getCursorDisplayName(_cursor)), cur_name, _cursor.kind, std::get<0>(cursor_pos), std::get<1>(cursor_pos), std::get<2>(cursor_pos));
		return temp_node;
	}
	node_db::node_db()
	{

	}
}
#pragma once
#include <spdlog/spdlog.h>
#include <string>
#include <clang-c/Index.h>
#include <optional>

namespace meta::utils
{
	std::string to_string(const CXCursor& _in_cursor);
	std::string to_string(const CXString &str);
	std::string to_string(const CXType& _in_type);
	std::string to_string(CXTemplateArgumentKind _in_kind);
	std::string to_string(CXTypeKind _in_kind);
	const std::string& get_qualified_name_from_cursor(CXCursor in_cursor);
	std::string join(const std::vector<std::string>& param, const std::string& sep);
	const std::string& to_string(CXCursorKind _in_kind);
	spdlog::logger& get_logger();
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
	std::vector<CXCursor> cursor_get_children(CXCursor parent);
	CXTypeKind expect_std_int(CXType int_type);
}

#pragma once
#include <spdlog/spdlog.h>
#include <string>
#include <clang-c/Index.h>


namespace meta::utils
{
	std::string to_string(const CXString &str);
	std::string to_string(const CXType& _in_type);
	std::string to_string(CXTemplateArgumentKind _in_kind);
	std::string join(const std::vector<std::string>& param, const std::string& sep);
	const std::string& cursor_kind_to_string(CXCursorKind _in_kind);
	spdlog::logger& get_logger();
}

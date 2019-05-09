#pragma once
#include <spdlog/spdlog.h>
#include <string>
#include <clang-c/Index.h>


namespace meta::utils
{
	std::string to_string(const CXString &str);
	std::string join(const std::vector<std::string>& param, const std::string& sep);
	spdlog::logger& get_logger();
}

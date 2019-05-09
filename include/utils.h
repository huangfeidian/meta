#pragma once
#include <spdlog/spdlog.h>


namespace meta::utils
{
	std::string join(const std::vector<std::string>& param, const std::string& sep);
	spdlog::logger& get_logger();
}


#include "utils.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace
{
	std::shared_ptr<spdlog::logger> create_logger()
	{
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::warn);
		console_sink->set_pattern("[meta] [%^%l%$] %v");

		auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("meta.log", true);
		file_sink->set_level(spdlog::level::trace);
		auto logger = std::make_shared<spdlog::logger>("meta", spdlog::sinks_init_list{ console_sink, file_sink });
		logger->set_level(spdlog::level::trace);
		return logger;
	}
}

namespace meta::utils
{
	std::string join(const std::vector<std::string>& param, const std::string& sep)
	{
		std::string result = "";
		std::uint32_t total_size = 0;
		for(const auto& i : param)
		{
			total_size += i.size();
		}
		total_size += sep.size() * (std::max(param.size(), 1u) - 1);
		result.reserve(total_size);
		for (const auto& one_str : param)
		{
			if (!result.empty())
			{
				result += sep + one_str;
			}
			else
			{
				result = one_str;
			}
		}
		return result;
	}
	spdlog::logger & get_logger()
	{
		static std::shared_ptr<spdlog::logger> _logger = create_logger();
		return *_logger;
	}

}
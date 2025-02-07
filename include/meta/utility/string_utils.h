﻿#pragma once
#include <utility>
#include <functional>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <optional>
#include <type_traits>
#include <string_view>
#include <regex>
namespace spiritsaway::meta::utils
{
	class string_utils
	{
		// the annotation should be (item1, item2, item3(key_1, key_2, key_3 = value_3))
	private:
		static std::vector<std::string_view> get_tokens(const std::string& cur_annotation)
		{
			if (cur_annotation.empty())
			{
				return {};
			}
			std::vector<std::string_view> token_result = {};
			std::size_t begin_index = 0;
			for (std::size_t i = 0; i < cur_annotation.size(); i++)
			{
				auto cur_char = cur_annotation[i];
				switch (cur_char)
				{
				case ' ':
				case '\t':
					if (begin_index != i)
					{
						token_result.push_back(std::string_view(&cur_annotation[begin_index], i - begin_index));
					}
					begin_index = i + 1;
					break;
				case '(':
				case ')':
				case ',':
					if (begin_index != i)
					{
						token_result.push_back(std::string_view(&cur_annotation[begin_index], i - begin_index));
					}
					token_result.push_back(std::string_view(&cur_annotation[i], 1));
					begin_index = i + 1;
					break;
				default:
					break;
				}
			}
			if (begin_index != cur_annotation.size())
			{
				token_result.push_back(std::string_view(&cur_annotation[begin_index], cur_annotation.size() - begin_index));
			}
			return token_result;
		}
	public:
		static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> parse_annotation(const std::string& cur_annotation)
		{
			const std::vector<std::string_view>& _tokens = get_tokens(cur_annotation);
			// auto& the_logger = meta::utils::get_logger();
			// the_logger.debug("parse_annotation for {} with token_size {}", cur_annotation, _tokens.size());
			std::unordered_map<std::string, std::unordered_map<std::string, std::string>> result;
			bool wait_for_right_paren = false;
			std::string cur_key;
			std::unordered_map<std::string, std::string> cur_values;
			for (auto token : _tokens)
			{
				switch (token.size())
				{
				case 1:
					switch (token[0])
					{
					case '(':
						if (wait_for_right_paren || cur_key.empty() || !cur_values.empty())
						{
							// the_logger.error("fail to parse annotation from {}", cur_annotation);
							return {};
						}
						wait_for_right_paren = true;
						break;
					case ')':
						if (wait_for_right_paren)
						{
							wait_for_right_paren = false;
							result[cur_key] = cur_values;
							cur_values.clear();
							cur_key.clear();
						}
						else
						{
							// the_logger.error("fail to parse annotation from {}", cur_annotation);
							return {};
						}
						break;
					case ',':
						if (!wait_for_right_paren)
						{
							if (!cur_key.empty())
							{
								result[cur_key] = {};
								cur_key.clear();
							}
							
						}
						break;
					default:
						if (wait_for_right_paren)
						{
							auto split_pos = token.find('=');
							if(split_pos != std::string::npos)
							{
								auto temp_key = std::string(token.substr(0, split_pos));
								auto temp_value = std::string(token.substr(split_pos + 1));
								cur_values[temp_key] = temp_value;
							}
							else
							{
								cur_values[std::string(token)] = "";
							}
						}
						else
						{
							cur_key = std::string(token);
						}
						break;
					}
					break;
				default:
					if (wait_for_right_paren)
					{
						auto split_pos = token.find('=');
							if(split_pos != std::string::npos)
							{
								auto temp_key = std::string(token.substr(0, split_pos));
								auto temp_value = std::string(token.substr(split_pos + 1));
								cur_values[temp_key] = temp_value;
							}
							else
							{
								cur_values[std::string(token)] = "";
							}
					}
					else
					{
						cur_key = std::string(token);
					}
					break;
				}

			}
			if (wait_for_right_paren)
			{
				// the_logger.error("fail to parse annotation from {}", cur_annotation);
				return {};
			}
			if (!cur_key.empty())
			{
				result[cur_key] = {};
			}
			return result;
		}
		static std::string_view strip_blank(std::string_view input)
		{
			std::size_t left = 0;
			std::size_t right = input.size();
			while (left < right)
			{
				auto cur_char = input[left];
				if (cur_char == ' ' || cur_char == '\t' || cur_char == '\n')
				{
					left++;
				}
				else
				{
					break;
				}
			}
			while (left < right)
			{
				
				auto cur_char = input[right - 1];
				if (cur_char == ' ' || cur_char == '\t' || cur_char == '\n')
				{
					right--;
				}
				else
				{
					break;
				}
			}
			if (left == right)
			{
				return std::string_view();
			}
			else
			{
				return input.substr(left, right - left);
			}
		}
		//remove const volative reference of a typename 
		static std::string_view remove_cvr(std::string_view cur_type_name)
		{
			cur_type_name = strip_blank(cur_type_name);
			if(cur_type_name.empty())
			{
				return cur_type_name;
			}
			if(starts_with(cur_type_name, "const "))
			{
				return remove_cvr(cur_type_name.substr(6));
			}
			if(starts_with(cur_type_name, "volatile "))
			{
				return remove_cvr(cur_type_name.substr(9));
			}
			if(cur_type_name.back() == '&')
			{
				cur_type_name.remove_suffix(1);
				return remove_cvr(cur_type_name);
			}
			return cur_type_name;
		}
		static bool starts_with(std::string_view content, std::string_view pattern)
		{
			return content.rfind(pattern, 0) == 0;
		}
		static bool ends_with(std::string_view content, std::string_view pattern)
		{
			if(pattern.size() > content.size()) return false;
			return std::equal(content.rbegin(), pattern.rend(), pattern.rbegin());
		
		}
		static std::string join(const std::vector<std::string_view>& tokens, std::string_view glue)
		{
			std::size_t total_size = 0;
			if (tokens.empty())
			{
				return std::string();
			}
			for (auto i : tokens)
			{
				total_size += i.size();
			}
			total_size += (tokens.size() - 1) * glue.size();
			std::string result;
			result.reserve(total_size);
			for (std::size_t i = 0; i < tokens.size(); i++)
			{
				result += tokens[i];
				if (i != tokens.size() - 1)
				{
					result += glue;
				}
			}
			return result;

		}
		static std::string replace(const std::string& content, const std::string& match_pattern, const std::string& replace_pattern)
		{
			std::vector<std::string_view> tokens;
			auto match_size = match_pattern.size();
			if (match_pattern.empty())
			{
				return content;
			}
			if (content.size() < match_pattern.size())
			{
				return content;
			}
			std::size_t index = 0;
			while (true)
			{
				auto new_index = content.find(match_pattern, index);
				if (new_index == std::string::npos)
				{
					tokens.push_back(std::string_view(content.data() + index, content.size() - index));
					break;
				}
				tokens.push_back(std::string_view(content.data() + index, new_index - index));
				index = new_index + match_size;
			}
				
			auto result = join(tokens, replace_pattern);
			return result;
		}

	};
}
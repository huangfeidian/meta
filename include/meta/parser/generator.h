#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <mustache.hpp>

#include "nodes/callable.h"
#include "nodes/enum.h"
#include "clang_utils.h"
namespace mustache = kainjow::mustache;
using namespace spiritsaway::meta::language;
namespace spiritsaway::meta::generator
{
	template<typename T>
	bool sort_by_unqualified_name(const T* a, const T* b)
	{
		return a->unqualified_name() < b->unqualified_name();
	}
	mustache::mustache load_mustache_from_file(const std::string& file_path)
	{
		auto _mustache_file = std::ifstream(file_path);
		std::string _template_str = std::string(std::istreambuf_iterator<char>(_mustache_file), std::istreambuf_iterator<char>());
		mustache::mustache _mustache_tempalte(_template_str);
		_mustache_file.close();
		return _mustache_tempalte;
	}
	void append_output_to_stream(std::unordered_map<std::string, std::string>& file_buffer, const std::string& file_name, const std::string& append_content)
	{
		auto cur_iter = file_buffer.find(file_name);
		if (cur_iter == file_buffer.end())
		{
			file_buffer[file_name] = append_content;
		}
		else
		{
			cur_iter->second += append_content;
		}
	}
	void write_content_to_file(const std::unordered_map<std::string, std::string>& file_buffer)
	{
		auto& the_logger = utils::get_logger();
		for (const auto&[file_name, file_content] : file_buffer)
		{
			if (!std::filesystem::exists(file_name))
			{
				the_logger.error("fail to write to no exist file {}", file_name);
				continue;
			}
			the_logger.info("write generate file {}", file_name);
			auto pre_file = std::ifstream(file_name);
			std::string pre_str = std::string(std::istreambuf_iterator<char>(pre_file), std::istreambuf_iterator<char>());
			pre_file.close();
			if (pre_str == file_content)
			{
				the_logger.info("write generate file {} ignored the content is same", file_name);
				return;
			}
			std::ofstream cur_file_stream(file_name);
			cur_file_stream << file_content;
			cur_file_stream.close();
		}
	}
	void merge_file_content(std::unordered_map<std::string, std::string>& final_result, const std::unordered_map<std::string, std::string>& append_result)
	{
		for (const auto&[file_name, file_content] : append_result)
		{
			append_output_to_stream(final_result, file_name, file_content);
		}
	}
	mustache::data generate_base_for_class(const class_node* one_class)
	{
		auto _bases = one_class->bases();
		std::sort(_bases.begin(), _bases.end(), sort_by_unqualified_name <language::type_info>);
		mustache::data base_list{ mustache::data::type::list };
		for (auto one_base : _bases)
		{
			base_list << mustache::data{ "base_type", one_base->name() };
		}
		return base_list;
	}


	
	
	using tag_func_desc = std::pair<const variable_node*, const callable_node*>;
	std::vector<tag_func_desc> parse_tag_func_for_class_with_field(const class_node* one_class, const std::string& field_tag, const std::string& tag)
	{
		std::unordered_map<std::string, std::string> components_value = {};
		std::unordered_map<std::string, std::string> func_call_annotate_value = {};
		auto field_pred = [&components_value, &field_tag](const variable_node& _cur_node)
		{
			return filter_with_annotation<variable_node>(field_tag, _cur_node);
		};
		auto func_pred = [&func_call_annotate_value, &tag](const callable_node& _cur_node)
		{
			return filter_with_annotation<callable_node>(tag, _cur_node);
		};
		std::vector<std::pair<const variable_node*, const callable_node*>> result;
		for (const auto& one_field_funcs : one_class->query_method_recursive_with_fields(func_pred, field_pred, false))
		{
			for (const auto& one_func : one_field_funcs.second)
			{
				result.emplace_back(one_field_funcs.first, one_func);
			}
		}
		
		std::sort(result.begin(), result.end(), [](const tag_func_desc& a, const tag_func_desc& b)
			{
				if (a.first && b.first)
				{
					return a.first->unqualified_name() < b.first->unqualified_name();
				}
				else
				{
					return a.second->unqualified_name() < b.second->unqualified_name();
				}
			});
		return result;
	}


	mustache::data generate_funcs_for_class_with_pred(const class_node* one_class, const std::function<bool(const callable_node&)>& func_pred)
	{
		const auto& filed_func_info = one_class->query_method_with_pred_recursive(func_pred);
		std::unordered_set<const callable_node*> local_func_set;
		for (auto one_func : one_class->query_method_with_pred(func_pred))
		{
			local_func_set.insert(one_func);
		}
		std::uint16_t func_method_idx = 0;
		std::size_t total_method_size = filed_func_info.size();
		mustache::data method_list{ mustache::data::type::list };
		for (auto  one_method : filed_func_info)
		{
			mustache::data cur_method_data;

			cur_method_data.set("func_index", std::to_string(func_method_idx));
			cur_method_data.set("func_name", one_method->func_name());
			cur_method_data.set("is_const_func", one_method->is_const_method());
			cur_method_data.set("is_inherited", local_func_set.find(one_method) == local_func_set.end());
			if (func_method_idx + 1 == total_method_size)
			{
				cur_method_data.set("last_func", true);
			}
			const auto& method_args = one_method->args_type();
			std::size_t arg_size = method_args.size();
			mustache::data arg_list{ mustache::data::type::list };
			std::size_t arg_idx = 0;
			for (auto one_arg : method_args)
			{
				mustache::data cur_arg_data;
				cur_arg_data.set("arg_idx", std::to_string(arg_idx));
				auto cur_arg_type = one_arg->decl_type();
				cur_arg_data.set("is_no_const_ref", cur_arg_type->is_lvalue_refer() && !cur_arg_type->is_const());
				cur_arg_data.set("arg_type", cur_arg_type->pretty_name());
				cur_arg_data.set("arg_name", one_arg->unqualified_name());
				
				if (arg_idx + 1 == arg_size)
				{
					cur_arg_data.set("last_idx", true);
				}
				arg_list << cur_arg_data;
				arg_idx += 1;

			}
			cur_method_data.set("func_args", arg_list);
			func_method_idx += 1;
			method_list << cur_method_data;
		}

		return method_list;
	}
	mustache::data generate_fields_for_class_with_pred(const class_node* one_class, const std::function<bool(const variable_node&)>& field_pred)
	{
		auto all_attr_fields = one_class->query_fields_with_pred_recursive(field_pred);
		std::unordered_set<const variable_node*>  local_attrs_set;
		for (auto one_attr : all_attr_fields)
		{
			local_attrs_set.insert(one_attr);
		}
		std::sort(all_attr_fields.begin(), all_attr_fields.end(), sort_by_unqualified_name<meta::language::variable_node>);
		mustache::data var_list{ mustache::data::type::list };
		for (std::size_t i = 0; i < all_attr_fields.size(); i++)
		{
			mustache::data temp_var;
			temp_var.set("var_idx", std::to_string(i));
			temp_var.set("var_name", all_attr_fields[i]->unqualified_name());
			temp_var.set("is_const", all_attr_fields[i]->decl_type()->is_const());
			temp_var.set("is_inherited", local_attrs_set.find(all_attr_fields[i]) == local_attrs_set.end());
			var_list << temp_var;
		}
		return var_list;
	}
	
	mustache::data generate_helper_for_enum(const enum_node* one_class)
	{
		mustache::data enum_items{ mustache::data::type::list };
		auto& enum_names = one_class->keys();
		for (std::size_t i = 0; i < enum_names.size(); i++)
		{
			mustache::data temp_item;
			temp_item.set("enum_name", enum_names[i]);
			temp_item.set("last_item", i == enum_names.size() - 1);
			enum_items << temp_item;
		}
		return enum_items;
	}
}

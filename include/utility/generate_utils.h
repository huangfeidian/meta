#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <mustache.hpp>

#include <nodes/callable.h>

#include "../utils.h"
namespace mustache = kainjow::mustache;
namespace meta::utils
{
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
		auto& the_logger = get_logger();
		for (const auto&[file_name, file_content] : file_buffer)
		{
			if (!std::filesystem::exists(file_name))
			{
				the_logger.error("fail to write to no exist file {}", file_name);
				continue;
			}
			the_logger.info("write generate file {}", file_name);
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
	template <typename T>
	bool filter_with_annotation_value(const std::string& _annotation_name, const std::vector<std::string>& _annotation_value, const T& _cur_node)
	{

		auto& _cur_annotations = _cur_node.annotations();
		auto cur_iter = _cur_annotations.find(_annotation_name);
		if (cur_iter == _cur_annotations.end())
		{
			return false;
		}
		if (cur_iter->second != _annotation_value)
		{
			return false;
		}
		return true;
	}
	template <typename T>
	bool filter_with_annotation(const std::string& _annotation_name, const T& _cur_node)
	{

		auto& _cur_annotations = _cur_node.annotations();
		auto cur_iter = _cur_annotations.find(_annotation_name);
		if (cur_iter == _cur_annotations.end())
		{
			return false;
		}
		return true;

	}
	std::string generate_encode_func_for_class(const language::class_node* one_class, mustache::mustache& mustache_template)
	{
		// 首先encode父类 按照父类的名称排序
		auto _bases = one_class->bases();
		std::sort(_bases.begin(), _bases.end(), [](const language::type_info* a, const language::type_info* b)
			{
				if (a->name() < b->name())
				{
					return true;
				}
				else
				{
					return false;
				}
			});
		mustache::data base_list{ mustache::data::type::list };
		for (auto one_base : _bases)
		{
			base_list << mustache::data{ "base_type", one_base->name() };
		}
		// 然后encode自己的变量
		std::vector<std::string> field_encode_value = {};
		auto encode_fields = one_class->query_fields_with_pred([&field_encode_value](const language::variable_node& _cur_node)
			{
				return filter_with_annotation_value<language::variable_node>("encode", field_encode_value, _cur_node);
			});
		std::sort(encode_fields.begin(), encode_fields.end(), [](const language::variable_node* a, const language::variable_node* b)
			{
				if (a->unqualified_name() < b->unqualified_name())
				{
					return true;
				}
				else
				{
					return false;
				}
			});
		mustache::data field_list{ mustache::data::type::list };
		for (auto one_field : encode_fields)
		{
			field_list << mustache::data{ "field_name", one_field->unqualified_name() };
		}
		mustache::data render_args;
		render_args.set("fields", field_list);
		render_args.set("bases", base_list);
		auto encode_str = mustache_template.render(render_args);
		return encode_str;
	}
	std::string generate_decode_func_for_class(const language::class_node* one_class, mustache::mustache& decode_template)
	{
		// 首先decode父类 按照父类的名称排序
		auto pre_bases = one_class->bases();
		std::vector<const language::type_info*> _bases;
		std::copy_if(pre_bases.begin(), pre_bases.end(), std::back_inserter(_bases), [](const language::type_info* _cur_node)
			{
				if (_cur_node->name()._Starts_with("std::"))
				{
					return true;
				}
				auto cur_related_class = _cur_node->related_class();
				if (!cur_related_class)
				{
					return false;
				}
				if (filter_with_annotation<language::class_node>("encode", *cur_related_class))
				{
					return true;
				}
				return false;
			});
		std::sort(_bases.begin(), _bases.end(), [](const language::type_info* a, const language::type_info* b)
			{
				if (a->name() < b->name())
				{
					return true;
				}
				else
				{
					return false;
				}
			});
		std::vector<std::string> field_decode_value = {};
		auto decode_fields = one_class->query_fields_with_pred([&field_decode_value](const language::variable_node& _cur_node)
			{
				return filter_with_annotation_value<language::variable_node>("decode", field_decode_value, _cur_node);
			});
		std::sort(decode_fields.begin(), decode_fields.end(), [](const language::variable_node* a, const language::variable_node* b)
			{
				if (a->unqualified_name() < b->unqualified_name())
				{
					return true;
				}
				else
				{
					return false;
				}

			});

		mustache::data base_list{ mustache::data::type::list };
		std::size_t decode_idx = 0;
		for (auto one_base : _bases)
		{
			auto _one_class = one_base->related_class();

			if (filter_with_annotation<language::class_node>("decode", *_one_class))
			{

				// 默认encode 与decode 的需求格式统一
				mustache::data base_arg;
				base_arg.set("idx", std::to_string(decode_idx));
				base_arg.set("base_type", _one_class->name());
				base_list << base_arg;
			}

			decode_idx += 1;
		}
		mustache::data field_list{ mustache::data::type::list };

		for (auto one_field : decode_fields)
		{
			if (filter_with_annotation<language::variable_node>("decode", *one_field))
			{
				mustache::data field_arg;
				field_arg.set("idx", std::to_string(decode_idx));
				field_arg.set("field_name", one_field->unqualified_name());
				field_list << field_arg;
			}
			decode_idx += 1;
		}
		mustache::data render_args;
		render_args.set("fields", field_list);
		render_args.set("bases", base_list);
		render_args.set("total_size", std::to_string(decode_idx));
		auto decode_str = decode_template.render(render_args);
		return decode_str;
	}
	std::string generate_property_func_for_class(const language::class_node* one_class, mustache::mustache& property_proxy_template, mustache::mustache& property_sequence_template)
	{
		// 生成一个类的所有property信息
		auto& the_logger = utils::get_logger();
		std::vector<std::string> property_annotate_value;
		auto property_fields = one_class->query_fields_with_pred([&property_annotate_value](const language::variable_node& _cur_node)
			{
				return utils::filter_with_annotation_value<language::variable_node>("property", property_annotate_value, _cur_node);
			});
		auto property_fields_with_base = one_class->query_fields_with_pred_recursive([&property_annotate_value](const language::variable_node& _cur_node)
			{
				return utils::filter_with_annotation_value<language::variable_node>("property", property_annotate_value, _cur_node);
			});
		std::sort(property_fields.begin(), property_fields.end(), [](const language::variable_node* a, const language::variable_node* b)
			{
				if (a->unqualified_name() < b->unqualified_name())
				{
					return true;
				}
				else
				{
					return false;
				}
			});
		std::size_t field_begin_index = property_fields_with_base.size() - property_fields.size();
		std::ostringstream h_stream;
		std::ostringstream cpp_stream;
		std::string cur_class_name = one_class->qualified_name();
		auto base_classes = one_class->base_classes();
		if (base_classes.size() > 1)
		{
			the_logger.error("cant generate property for class {} with {} base classes", cur_class_name, base_classes.size());
			return "";
		}
		mustache::data render_args;
		if (base_classes.size() == 1)
		{
			render_args.set("base_class", true);
			render_args.set("base_type", base_classes[0]->unqualified_name());
			render_args.set("base_idx", std::to_string(field_begin_index));
		}
		mustache::data field_list{ mustache::data::type::list };
		for (auto one_field : property_fields)
		{
			mustache::data cur_field_render_arg;
			auto cur_field_name = one_field->unqualified_name();
			auto cur_field_type_name = one_field->decl_type()->name();
			cur_field_render_arg.set("field_name", cur_field_name);
			cur_field_render_arg.set("field_idx", std::to_string(field_begin_index));
			field_list << cur_field_render_arg;
			field_begin_index++;
		}
		render_args.set("fields", field_list);
		return property_proxy_template.render(render_args) + property_sequence_template.render(render_args);

	}
	std::string generate_rpc_call_for_class(const language::class_node* one_class, mustache::mustache& rpc_call_tempalte)
	{
		
		std::vector<std::string> rpc_call_annotate_value = {};
		auto rpc_methods = one_class->query_method_with_pred_recursive([&rpc_call_annotate_value](const language::callable_node& _cur_node)
			{
				return filter_with_annotation_value<language::callable_node>("rpc", rpc_call_annotate_value, _cur_node);
			});
		std::sort(rpc_methods.begin(), rpc_methods.end(), [](const language::callable_node* a, const language::callable_node* b)
			{
				if (a->unqualified_name() < b->unqualified_name())
				{
					return true;
				}
				else
				{
					return false;
				}

			});
		std::uint16_t rpc_method_idx = 0;
		std::size_t total_rpc_method_size = rpc_methods.size();
		mustache::data method_list{ mustache::data::type::list };
		for (auto one_method : rpc_methods)
		{
			mustache::data cur_method_data;
			cur_method_data.set("rpc_index", std::to_string(rpc_method_idx));
			cur_method_data.set("rpc_name", one_method->func_name());
			if (rpc_method_idx + 1 == total_rpc_method_size)
			{
				cur_method_data.set("last_rpc", true);
			}
			const auto& method_args = one_method->args_type();
			std::size_t arg_size = method_args.size();
			mustache::data arg_list{ mustache::data::type::list };
			std::size_t arg_idx = 0;
			for (auto one_arg : method_args)
			{
				mustache::data cur_arg_data;
				cur_arg_data.set("arg_idx", std::to_string(arg_idx));
				cur_arg_data.set("arg_type", one_arg->decl_type()->name());
				if (arg_idx + 1 == arg_size)
				{
					cur_arg_data.set("last_idx", true);
				}
				arg_list << cur_arg_data;
				arg_idx += 1;

			}
			cur_method_data.set("rpc_args", arg_list);
			rpc_method_idx += 1;
			method_list << cur_method_data;
		}
		mustache::data render_args;
		render_args.set("rpc_methods", method_list);


		return rpc_call_tempalte.render(render_args);
	}

}

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
using namespace meta::language;
namespace meta::utils
{
	template<typename T>
	bool sort_by_unqualified_name(const T* a, const T* b)
	{
		return a->unqualified_name() < b->unqualified_name();
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

	mustache::data generate_encode_func_for_class(const class_node* one_class)
	{
		// 首先encode父类 按照父类的名称排序
		auto _bases = one_class->bases();
		std::sort(_bases.begin(), _bases.end(), sort_by_unqualified_name <language::type_info>);
		mustache::data base_list{ mustache::data::type::list };
		for (auto one_base : _bases)
		{
			base_list << mustache::data{ "base_type", one_base->name() };
		}
		// 然后encode自己的变量
		std::unordered_map<std::string, std::string> field_encode_value = {};
		auto encode_fields = one_class->query_fields_with_pred([&field_encode_value](const variable_node& _cur_node)
			{
				return filter_with_annotation_value<variable_node>("encode", field_encode_value, _cur_node);
			});
		std::sort(encode_fields.begin(), encode_fields.end(), sort_by_unqualified_name<language::variable_node>);
		mustache::data field_list{ mustache::data::type::list };
		for (auto one_field : encode_fields)
		{
			field_list << mustache::data{ "field_name", one_field->unqualified_name() };
		}
		mustache::data render_args;
		render_args.set("fields", field_list);
		render_args.set("bases", base_list);
		return render_args;
	}
	mustache::data generate_decode_func_for_class(const class_node* one_class)
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
				if (filter_with_annotation<class_node>("encode", *cur_related_class))
				{
					return true;
				}
				return false;
			});
		std::sort(_bases.begin(), _bases.end(), sort_by_unqualified_name<language::type_info>);
		std::unordered_map<std::string, std::string> field_decode_value = {};
		auto decode_fields = one_class->query_fields_with_pred([&field_decode_value](const variable_node& _cur_node)
			{
				return filter_with_annotation_value<variable_node>("decode", field_decode_value, _cur_node);
			});
		std::sort(decode_fields.begin(), decode_fields.end(), sort_by_unqualified_name<language::variable_node>);

		mustache::data base_list{ mustache::data::type::list };
		std::size_t decode_idx = 0;
		for (auto one_base : _bases)
		{
			auto _one_class = one_base->related_class();

			if (filter_with_annotation<class_node>("decode", *_one_class))
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
			if (filter_with_annotation<variable_node>("decode", *one_field))
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
		return render_args;
	}
	mustache::data generate_property_func_for_class(const class_node* one_class)
	{
		// 生成一个类的所有property信息
		auto& the_logger = utils::get_logger();
		std::unordered_map<std::string, std::string> property_annotate_value;
		auto property_fields = one_class->query_fields_with_pred([&property_annotate_value](const variable_node& _cur_node)
			{
				return filter_with_annotation_value<variable_node>("property", property_annotate_value, _cur_node);
			});
		auto property_fields_with_base = one_class->query_fields_with_pred_recursive([&property_annotate_value](const variable_node& _cur_node)
			{
				return filter_with_annotation_value<variable_node>("property", property_annotate_value, _cur_node);
			});
		std::sort(property_fields.begin(), property_fields.end(), sort_by_unqualified_name<language::variable_node>);
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
		return render_args;

	}
	mustache::data generate_rpc_call_for_class(const class_node* one_class)
	{
		std::unordered_map<std::string, std::string> rpc_call_annotate_value = {};
		auto func_pred = [&rpc_call_annotate_value](const callable_node& _cur_node)
		{
			return filter_with_annotation_value<callable_node>("rpc", rpc_call_annotate_value, _cur_node);
		};
		
		
		auto rpc_methods = one_class->query_method_with_pred_recursive(func_pred);
		std::sort(rpc_methods.begin(), rpc_methods.end(), sort_by_unqualified_name<language::callable_node>);
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
				cur_arg_data.set("arg_name", one_arg->unqualified_name());
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


		return render_args;
	}
	mustache::data generate_components_add_for_class(const class_node* one_class)
	{
		std::unordered_map<std::string, std::string> components_value = {};
		auto field_pred = [&components_value](const variable_node& _cur_node)
		{
			return filter_with_annotation_value<variable_node>("component", components_value, _cur_node);
		};
		auto component_fields = one_class->query_fields_with_pred_recursive(field_pred);
		std::sort(component_fields.begin(), component_fields.end(), sort_by_unqualified_name<language::variable_node>);

		mustache::data components{ mustache::data::type::list };
		for (auto one_field : component_fields)
		{
			mustache::data cur_component;
			cur_component.set("name", one_field->unqualified_name());
			components << cur_component;
		}
		mustache::data component_args;
		component_args.set("components", components);
		return component_args;
	}
	mustache::data generate_stub_func_for_class(const class_node* one_class)
	{
		std::unordered_map<std::string, std::string> components_value = {};
		auto field_pred = [&components_value](const variable_node& _cur_node)
		{
			return filter_with_annotation_value<variable_node>("component", components_value, _cur_node);
		};
		auto func_pred = [&components_value](const callable_node& _cur_node)
		{
			return filter_with_annotation_value<callable_node>("stub_func", components_value, _cur_node);
		};
		auto stub_funcs = one_class->query_method_recursive_with_fields(func_pred, field_pred, false);
		auto root_stub_funcs = stub_funcs[nullptr];
		stub_funcs.erase(nullptr);
		// 生成所有的公共入口函数 及对应的优先级
		mustache::data stub_func_render_args{ mustache::data::type::list };
		std::unordered_map<std::string, std::unordered_map<std::string, int>> component_stub_funcs;
		for (const auto& one_field_funcs: stub_funcs)
		{
			auto& cur_class_stub_funcs = one_field_funcs.second;
			std::unordered_map<std::string, int> cur_stub_funcs;
			for (auto one_func : cur_class_stub_funcs)
			{
				auto cur_priority_opt_str = one_func->get_anotation_detail_value("stub_func", "priority");
				int cur_priority = 0;
				if (cur_priority_opt_str)
				{
					cur_priority = std::stoi(cur_priority_opt_str.value());
				}
				cur_stub_funcs[one_func->func_name()] = cur_priority;
			}
			component_stub_funcs[one_field_funcs.first->unqualified_name()] = cur_stub_funcs;

		}
		for (auto one_func : root_stub_funcs)
		{
			mustache::data cur_stub_funcs{ mustache::data::type::list };
			const auto& cur_func_name = one_func->func_name();
			std::vector<std::pair<int, std::string>> _fields_with_priority;
			for (const auto& one_component : component_stub_funcs)
			{
				auto cur_iter = one_component.second.find(cur_func_name);
				if (cur_iter != one_component.second.end())
				{
					_fields_with_priority.emplace_back(cur_iter->second, one_component.first);
				}
			}
			
			std::sort(_fields_with_priority.begin(), _fields_with_priority.end());
			std::reverse(_fields_with_priority.begin(), _fields_with_priority.end());
			for (const auto& one_component : _fields_with_priority)
			{
				mustache::data temp1;
				temp1.set("priority", std::to_string(one_component.first));
				temp1.set("name", one_component.second);
				cur_stub_funcs << temp1;
			}
			mustache::data temp2;
			temp2.set("stub_interface_name", cur_func_name);
			temp2.set("components", cur_stub_funcs);
			stub_func_render_args << temp2;
		}
		mustache::data temp3;
		temp3.set("stub_interface", stub_func_render_args);
		return temp3;
	}
	using tag_func_desc = std::pair<const variable_node*, const callable_node*>;
	std::vector<tag_func_desc> parse_tag_func_for_class_with_components(const class_node* one_class, const std::string& tag)
	{
		std::unordered_map<std::string, std::string> components_value = {};
		std::unordered_map<std::string, std::string> func_call_annotate_value = {};
		auto field_pred = [&components_value](const variable_node& _cur_node)
		{
			return filter_with_annotation<variable_node>("component", _cur_node);
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
				if (a.first->unqualified_name() < b.first->unqualified_name())
				{
					return true;
				}
				else
				{
					return a.second->unqualified_name() < b.second->unqualified_name();
				}
			});
		return result;
	}
	std::vector<std::pair<const variable_node*, const callable_node*>> parse_rpc_func_for_class(const class_node* one_class)
	{
		return parse_tag_func_for_class_with_components(one_class, "rpc");
	}
	std::vector<std::pair<const variable_node*, const callable_node*>> parse_interface_func_for_class(const class_node* one_class)
	{
		return parse_tag_func_for_class_with_components(one_class, "interface");
	}


	mustache::data generate_func_interface_for_component_class(const std::vector<tag_func_desc>& filed_func_info)
	{
		std::uint16_t func_method_idx = 0;
		std::size_t total_method_size = filed_func_info.size();
		mustache::data method_list{ mustache::data::type::list };
		for (auto [field, one_method] : filed_func_info)
		{
			mustache::data cur_method_data;
			std::string method_full_name;
			if (field)
			{
				method_full_name = field->unqualified_name() + "::" + one_method->unqualified_name();
			}
			else
			{
				method_full_name = one_method->unqualified_name();
			}
			cur_method_data.set("func_index", std::to_string(func_method_idx));
			cur_method_data.set("func_name", method_full_name);
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
				cur_arg_data.set("arg_type", one_arg->decl_type()->name());
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
		mustache::data render_args;
		render_args.set("func_methods", method_list);
		return render_args;
	}
}

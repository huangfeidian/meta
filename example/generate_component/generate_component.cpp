
#include <iostream>
#include <iomanip>

#include <cstdint>
#include <fstream>
#include <iomanip>

#include <iostream>
#include <sstream>
#include <queue>
#include <filesystem>
#include <meta/parser/nodes/class.h>
#include <meta/parser/clang_utils.h>

//#include <meta/serialize/decode.h>
#include <meta/parser/generator.h>


using namespace std;
using namespace spiritsaway::meta;
using namespace spiritsaway::meta::generator;

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
	for (const auto& one_field_funcs : stub_funcs)
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
	return stub_func_render_args;
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
			cur_arg_data.set("arg_type", one_arg->decl_type()->pretty_name());
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

	return method_list;
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
	return components;
}

std::unordered_map<std::string, std::string> generate_component()
{
	auto& the_logger = utils::get_logger();
	std::unordered_map<std::string, std::string> _annotation_value = { };
	auto all_property_classes = language::type_db::instance().get_class_with_pred([&_annotation_value](const language::class_node& _cur_node)
		{
			return language::filter_with_annotation_value<language::class_node>(std::string("component_owner"), _annotation_value, _cur_node);
		});
	std::unordered_map<std::string, std::string> result;

	mustache::mustache component_mustache_tempalte = generator::load_mustache_from_file("../mustache/component.mustache");

	mustache::mustache stub_interface_mustache_tempalte = generator::load_mustache_from_file("../mustache/stub_interface.mustache");
	mustache::mustache rpc_mustache_tempalte = generator::load_mustache_from_file("../mustache/rpc_call.mustache");


	for (auto one_class : all_property_classes)
	{
		auto cur_file_path_str = one_class->file();
		std::filesystem::path file_path(cur_file_path_str);
		auto _cur_parent_path = file_path.parent_path();
		auto generated_h_file_name = one_class->unqualified_name() + ".generated_h";
		auto new_h_file_path = _cur_parent_path / generated_h_file_name;
		auto generated_cpp_file_name = one_class->unqualified_name() + ".generated_cpp";
		auto new_cpp_file_path = _cur_parent_path / generated_cpp_file_name;
		auto component_data = generate_components_add_for_class(one_class);
		auto stub_func_data = generate_stub_func_for_class(one_class);
		mustache::data render_args;
		render_args.set("class_name", one_class->unqualified_name());
		render_args.set("class_full_name", one_class->qualified_name());
		render_args.set("components", component_data);
		render_args.set("stub_interface", stub_func_data);
		render_args.set("rpc_methods", generate_rpc_call_for_class(one_class));
		generator::append_output_to_stream(result, new_h_file_path.string(), component_mustache_tempalte.render(render_args));
		generator::append_output_to_stream(result, new_cpp_file_path.string(), stub_interface_mustache_tempalte.render(render_args));
		generator::append_output_to_stream(result, new_h_file_path.string(), rpc_mustache_tempalte.render(render_args));
	}
	return result;
}
int main()
{
	auto& the_logger = utils::get_logger();
	CXIndex m_index;
	CXTranslationUnit m_translationUnit;
	std::vector<std::string> arguments;
	arguments.push_back("-x");
	arguments.push_back("c++");
	arguments.push_back("-std=c++17");
	arguments.push_back("-D__meta_parse__");
	arguments.push_back("-ID:/usr/include/");
	arguments.push_back("-I../include/");
	std::vector<const char *> cstr_arguments;

	for (const auto& i : arguments)
	{
		cstr_arguments.push_back(i.c_str());
	}

	bool display_diag = true;
	m_index = clang_createIndex(true, display_diag);
	std::string file_path = "../example/generate_component/avatar.cpp";
	//std::string file_path = "sima.cpp";
	m_translationUnit = clang_createTranslationUnitFromSourceFile(m_index, file_path.c_str(), static_cast<int>(cstr_arguments.size()), cstr_arguments.data(), 0, nullptr);
	auto cursor = clang_getTranslationUnitCursor(m_translationUnit);
	the_logger.info("the root cursor is {}", utils::to_string(cursor));
	auto& cur_type_db = language::type_db::instance();
	cur_type_db.create_from_translate_unit(cursor);
	//recursive_print_decl_under_namespace("A");
	cur_type_db.build_class_under_namespace("std");
	cur_type_db.build_class_under_namespace("test");
	//recursive_print_func_under_namespace("A");
	//recursive_print_class_under_namespace("A");
	json result = language::type_db::instance().to_json();
	ofstream json_out("type_info.json");
	json_out << setw(4) << result << endl;
	std::unordered_map<std::string, std::string> file_content;
	//utils::merge_file_content(file_content, generate_encode_decode());
	generator::merge_file_content(file_content, generate_component());
	generator::write_content_to_file(file_content);
	clang_disposeTranslationUnit(m_translationUnit);

	return 0;

}
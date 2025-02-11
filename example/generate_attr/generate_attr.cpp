﻿
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


mustache::data generate_register_types_for_class(const class_node* one_class)
{
	mustache::data var_list{ mustache::data::type::list };
	std::unordered_set<std::string> base_type_set;
	auto field_pred = [](const variable_node& _cur_node)
	{
		return filter_with_annotation<variable_node>("attr", _cur_node);
	};
	auto func_pred = [](const callable_node& _cur_node)
	{
		return filter_with_annotation<callable_node>("attr", _cur_node);
	};
	auto all_attr_fields = one_class->query_fields_with_pred_recursive(field_pred);
	for (auto one_var : all_attr_fields)
	{
		base_type_set.insert(std::string(utils::string_utils::remove_cvr(one_var->decl_type()->pretty_name())));
	}
	const auto& attr_funcs_info = one_class->query_method_with_pred_recursive(func_pred);
	for (auto one_method : attr_funcs_info)
	{
		const auto& method_args = one_method->args_type();
		for (auto one_arg : method_args)
		{
			base_type_set.insert(std::string(utils::string_utils::remove_cvr(one_arg->decl_type()->pretty_name())));
		}
	}
	base_type_set.insert(one_class->qualified_name());
	for (const auto& one_type : base_type_set)
	{
		mustache::data temp_type;
		temp_type.set("type_name", one_type);
		var_list << temp_type;
	}
	return var_list;
}

std::unordered_map<std::string, std::string> generate_attr()
{
	auto& the_logger = utils::get_logger();
	std::unordered_map<std::string, std::string> _annotation_value = { };
	auto all_property_classes = language::type_db::instance().get_class_with_pred([&_annotation_value](const language::class_node& _cur_node)
		{
			return language::filter_with_annotation_value<language::class_node>("attr", _annotation_value, _cur_node);
		});
	std::unordered_map<std::string, std::string> result;
	auto attr_call_decl_mustache = generator::load_mustache_from_file("../mustache/attr_call_decl.mustache");
	auto attr_call_impl_mustache = generator::load_mustache_from_file("../mustache/attr_call_impl.mustache");
	auto static_constructor_decl_mustache = generator::load_mustache_from_file("../mustache/static_constructor_decl.mustache");
	auto static_constructor_impl_mustache = generator::load_mustache_from_file("../mustache/static_constructor_impl.mustache");
	auto attr_var_decl_mustache = generator::load_mustache_from_file("../mustache/attr_var_decl.mustache");
	auto attr_var_impl_mustache = generator::load_mustache_from_file("../mustache/attr_var_impl.mustache");
	auto register_type_decl_mustache = generator::load_mustache_from_file("../mustache/register_type_decl.mustache");
	auto register_type_impl_mustache = generator::load_mustache_from_file("../mustache/register_type_impl.mustache");

	auto func_pred = [](const callable_node& _cur_node)
	{
		return filter_with_annotation<callable_node>("attr", _cur_node);
	};
	auto field_pred = [](const variable_node& _cur_node)
	{
		return filter_with_annotation<variable_node>("attr", _cur_node);
	};
	for (auto one_class : all_property_classes)
	{
		auto cur_file_path_str = one_class->file();
		std::filesystem::path file_path(cur_file_path_str);
		auto _cur_parent_path = file_path.parent_path();
		auto generated_h_file_name = one_class->unqualified_name() + ".generated_h";
		auto new_h_file_path = _cur_parent_path / generated_h_file_name;
		auto generated_cpp_file_name = one_class->unqualified_name() + ".generated_cpp";
		auto new_cpp_file_path = _cur_parent_path / generated_cpp_file_name;
		mustache::data render_args;
		auto attr_func_args = generator::generate_funcs_for_class_with_pred(one_class, func_pred);
		auto attr_var_args = generator::generate_fields_for_class_with_pred(one_class, field_pred);
		auto base_types = generate_register_types_for_class(one_class);
		render_args.set("attr_funcs", attr_func_args);
		render_args.set("attr_vars", attr_var_args);
		render_args.set("register_base_types", base_types);
		render_args.set("class_name", one_class->unqualified_name());
		render_args.set("class_full_name", one_class->qualified_name());
		mustache::data static_construct_funcs{ mustache::data::type::list };
		std::vector<std::string> func_names;
		func_names.push_back("init_attr_func_map");
		func_names.push_back("init_attr_var_map");
		func_names.push_back("register_types");
		std::sort(func_names.begin(), func_names.end());
		for (const auto& one_func : func_names)
		{
			mustache::data temp_data;
			temp_data.set("func_name", one_func);
			static_construct_funcs << temp_data;

		}
		render_args.set("static_construct_funcs", static_construct_funcs);
		
		generator::append_output_to_stream(result, new_h_file_path.string(), static_constructor_decl_mustache.render(render_args));
		generator::append_output_to_stream(result, new_h_file_path.string(), attr_call_decl_mustache.render(render_args));
		generator::append_output_to_stream(result, new_cpp_file_path.string(), attr_call_impl_mustache.render(render_args));
		generator::append_output_to_stream(result, new_h_file_path.string(), attr_var_decl_mustache.render(render_args));
		generator::append_output_to_stream(result, new_cpp_file_path.string(), attr_var_impl_mustache.render(render_args));
		generator::append_output_to_stream(result, new_h_file_path.string(), register_type_decl_mustache.render(render_args));
		generator::append_output_to_stream(result, new_cpp_file_path.string(), register_type_impl_mustache.render(render_args));
		generator::append_output_to_stream(result, new_cpp_file_path.string(), static_constructor_impl_mustache.render(render_args));
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
	arguments.push_back("-I../../include/");
	std::vector<const char *> cstr_arguments;

	for (const auto& i : arguments)
	{
		cstr_arguments.push_back(i.c_str());
	}

	bool display_diag = true;
	m_index = clang_createIndex(true, display_diag);
	std::string file_path = "../../example/generate_attr/test_class.cpp";
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
	generator::merge_file_content(file_content, generate_attr());
	generator::write_content_to_file(file_content);
	clang_disposeTranslationUnit(m_translationUnit);

	return 0;

}
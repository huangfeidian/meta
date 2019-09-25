
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

#include <meta/serialize/decode.h>
#include <meta/parser/generator.h>

using namespace std;
using namespace meta;




std::unordered_map<std::string, std::string> generate_behavior_actions()
{
	auto& the_logger = utils::get_logger();
	std::unordered_map<std::string, std::string> _annotation_value = { };
	auto all_agent_class = language::type_db::instance().get_class_with_pred([&_annotation_value](const language::class_node& _cur_node)
		{
			return language::filter_with_annotation_value<language::class_node>("behavior", _annotation_value, _cur_node);
		});
	std::unordered_map<std::string, std::string> result;
	auto behavior_action_decl_mustache = generator::load_mustache_from_file("../mustache/behavior_action_decl.mustache");
	auto behavior_action_impl_mustache = generator::load_mustache_from_file("../mustache/behavior_action_impl.mustache");
	auto static_constructor_decl_mustache = generator::load_mustache_from_file("../mustache/static_constructor_decl.mustache");
	auto static_constructor_impl_mustache = generator::load_mustache_from_file("../mustache/static_constructor_impl.mustache");
	auto func_pred = [](const callable_node& _cur_node)
	{
		return filter_with_annotation<callable_node>("behavior_action", _cur_node);
	};

	for (auto one_class : all_agent_class)
	{
		auto cur_file_path_str = one_class->file();
		std::filesystem::path file_path(cur_file_path_str);
		auto _cur_parent_path = file_path.parent_path();
		auto generated_h_file_name = one_class->unqualified_name() + ".generated_h";
		auto new_h_file_path = _cur_parent_path / generated_h_file_name;
		auto generated_cpp_file_name = one_class->unqualified_name() + ".generated_cpp";
		auto new_cpp_file_path = _cur_parent_path / generated_cpp_file_name;
		mustache::data render_args;
		auto behavior_action_func_args = generator::generate_funcs_for_class_with_pred(one_class, func_pred);
		render_args.set("behavior_actions", behavior_action_func_args);
		render_args.set("class_name", one_class->unqualified_name());
		render_args.set("class_full_name", one_class->qualified_name());
		mustache::data static_construct_funcs{ mustache::data::type::list };
		std::vector<std::string> func_names;
		func_names.push_back("init_action_funcs_map");
		std::sort(func_names.begin(), func_names.end());
		for (const auto& one_func : func_names)
		{
			mustache::data temp_data;
			temp_data.set("func_name", one_func);
			static_construct_funcs << temp_data;

		}
		render_args.set("static_construct_funcs", static_construct_funcs);
		
		generator::append_output_to_stream(result, new_h_file_path.string(), static_constructor_decl_mustache.render(render_args));
		generator::append_output_to_stream(result, new_h_file_path.string(), behavior_action_decl_mustache.render(render_args));
		generator::append_output_to_stream(result, new_cpp_file_path.string(), behavior_action_impl_mustache.render(render_args));
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
	arguments.push_back("-I../include/");
	std::vector<const char *> cstr_arguments;

	for (const auto& i : arguments)
	{
		cstr_arguments.push_back(i.c_str());
	}

	bool display_diag = true;
	m_index = clang_createIndex(true, display_diag);
	std::string file_path = "../test/behavior_tree/action_agent.cpp";
	//std::string file_path = "sima.cpp";
	m_translationUnit = clang_createTranslationUnitFromSourceFile(m_index, file_path.c_str(), static_cast<int>(cstr_arguments.size()), cstr_arguments.data(), 0, nullptr);
	auto cursor = clang_getTranslationUnitCursor(m_translationUnit);
	the_logger.info("the root cursor is {}", utils::to_string(cursor));
	auto& cur_type_db = language::type_db::instance();
	cur_type_db.create_from_translate_unit(cursor);
	//recursive_print_decl_under_namespace("A");
	cur_type_db.build_class_under_namespace("std");
	cur_type_db.build_class_under_namespace("behavior");
	//recursive_print_func_under_namespace("A");
	//recursive_print_class_under_namespace("A");
	json result = language::type_db::instance().to_json();
	ofstream json_out("type_info.json");
	json_out << setw(4) << result << endl;
	std::unordered_map<std::string, std::string> file_content;
	//utils::merge_file_content(file_content, generate_encode_decode());
	generator::merge_file_content(file_content, generate_behavior_actions());
	generator::write_content_to_file(file_content);
	clang_disposeTranslationUnit(m_translationUnit);

	return 0;

}
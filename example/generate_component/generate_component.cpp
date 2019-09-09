
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
		auto component_data = generator::generate_components_add_for_class(one_class);
		auto stub_func_data = generator::generate_stub_func_for_class(one_class);
		mustache::data render_args;
		render_args.set("class_name", one_class->unqualified_name());
		render_args.set("class_full_name", one_class->qualified_name());
		render_args.set("components", component_data);
		render_args.set("stub_interface", stub_func_data);
		render_args.set("rpc_methods", generator::generate_rpc_call_for_class(one_class));
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

#include <iostream>
#include <iomanip>

#include <cstdint>
#include <fstream>
#include <iomanip>

#include <iostream>
#include <sstream>
#include <queue>
#include <filesystem>

#include <mustache.hpp>

#include "nodes/type_info.h"
#include "name_space.h"
#include "utils.h"
#include "nodes/class.h"
#include "nodes/enum.h"
#include "nodes/variable.h"
#include <serialize/encode.h>
#include <utility/generate_utils.h>

using namespace std;
using namespace meta;



std::unordered_map<std::string, std::string> generate_encode()
{
	// 遍历所有的class 对于里面表明了需要生成encode的类进行处理
	auto& the_logger = utils::get_logger();
	std::unordered_map < std::string, std::string> _annotation_value = { {"auto", ""} };

	auto all_encode_classses = language::type_db::instance().get_class_with_pred([&_annotation_value](const language::class_node& _cur_node)
		{
			return language::filter_with_annotation_value<language::class_node>("encode", _annotation_value, _cur_node);
		});

	std::unordered_map<std::string, std::string> result;
	auto encode_func_mustache_file = std::ifstream("../mustache/encode_func.mustache");
	std::string template_str = std::string(std::istreambuf_iterator<char>(encode_func_mustache_file), std::istreambuf_iterator<char>());
	mustache::mustache encode_func_mustache_tempalte(template_str);
	for (auto one_class : all_encode_classses)
	{
		auto cur_file_path_str = one_class->file();
		the_logger.info("get class {} with annotation prop encode location {}", one_class->unqualified_name(), cur_file_path_str);
		std::filesystem::path file_path(cur_file_path_str);
		auto _cur_parent_path = file_path.parent_path();
		auto generated_h_file_name = one_class->unqualified_name() + ".generated_h";
		auto new_h_file_path = _cur_parent_path / generated_h_file_name;
		auto encode_args = utils::generate_encode_func_for_class(one_class);
		utils::append_output_to_stream(result, new_h_file_path.string(), encode_func_mustache_tempalte.render(encode_args));
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
	the_logger.info("arguments is {}", utils::join(arguments, ","));
	std::vector<const char *> cstr_arguments;

	for (const auto& i : arguments)
	{
		cstr_arguments.push_back(i.c_str());
	}
	
	bool display_diag = true;
	m_index = clang_createIndex(true, display_diag);
	std::string file_path = "../example/generate_encode/test_class.cpp";
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
	utils::merge_file_content(file_content, generate_encode());
	utils::write_content_to_file(file_content);
	clang_disposeTranslationUnit(m_translationUnit);

	return 0;
    
}
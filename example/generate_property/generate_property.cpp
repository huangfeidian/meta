
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
	return field_list;

}


std::unordered_map<std::string, std::string> generate_property()
{
	auto& the_logger = utils::get_logger();
	std::unordered_map<std::string, std::string> _annotation_value = { {"auto", ""} };
	auto all_property_classes = language::type_db::instance().get_class_with_pred([&_annotation_value](const language::class_node& _cur_node)
		{
			return language::filter_with_annotation_value<language::class_node>("property", _annotation_value, _cur_node);
		});
	std::unordered_map<std::string, std::string> result;
	auto property_proxy_mustache_file = std::ifstream("../mustache/property_proxy.mustache");
	std::string property_proxy_template_str = std::string(std::istreambuf_iterator<char>(property_proxy_mustache_file), std::istreambuf_iterator<char>());
	mustache::mustache property_proxy_mustache_tempalte(property_proxy_template_str);

	auto property_sequence_mustache_file = std::ifstream("../mustache/property_sequence.mustache");
	std::string property_sequence_template_str = std::string(std::istreambuf_iterator<char>(property_sequence_mustache_file), std::istreambuf_iterator<char>());
	mustache::mustache property_sequence_mustache_tempalte(property_sequence_template_str);

	for (auto one_class : all_property_classes)
	{
		auto cur_file_path_str = one_class->file();
		std::filesystem::path file_path(cur_file_path_str);
		auto _cur_parent_path = file_path.parent_path();
		auto generated_h_file_name = one_class->unqualified_name() + ".generated_h";
		auto new_h_file_path = _cur_parent_path / generated_h_file_name;
		auto property_func_args = generate_property_func_for_class(one_class);
		mustache::data render_args;
		render_args.set("class_name", one_class->unqualified_name());
		render_args.set("class_full_name", one_class->qualified_name());
		render_args.set("property_fields", property_func_args);
		generator::append_output_to_stream(result, new_h_file_path.string(), property_proxy_mustache_tempalte.render(render_args));
		generator::append_output_to_stream(result, new_h_file_path.string(), property_sequence_mustache_tempalte.render(render_args));
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
	std::string file_path = "../example/generate_property/test_property.cpp";
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
	generator::merge_file_content(file_content, generate_property());
	generator::write_content_to_file(file_content);
	clang_disposeTranslationUnit(m_translationUnit);

	return 0;

}
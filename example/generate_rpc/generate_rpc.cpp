
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


std::unordered_map<std::string, std::string> generate_rpc()
{
	auto& the_logger = utils::get_logger();
	std::unordered_map<std::string, std::string> _annotation_value = { };
	auto all_property_classes = language::type_db::instance().get_class_with_pred([&_annotation_value](const language::class_node& _cur_node)
		{
			return language::filter_with_annotation_value<language::class_node>("rpc", _annotation_value, _cur_node);
		});
	std::unordered_map<std::string, std::string> result;
	auto rpc_call_mustache_file = std::ifstream("../mustache/rpc_call.mustache");
	std::string rpc_call_template_str = std::string(std::istreambuf_iterator<char>(rpc_call_mustache_file), std::istreambuf_iterator<char>());
	mustache::mustache rpc_call_mustache_tempalte(rpc_call_template_str);


	for (auto one_class : all_property_classes)
	{
		auto cur_file_path_str = one_class->file();
		std::filesystem::path file_path(cur_file_path_str);
		auto _cur_parent_path = file_path.parent_path();
		auto generated_h_file_name = one_class->unqualified_name() + ".generated_h";
		auto new_h_file_path = _cur_parent_path / generated_h_file_name;
		mustache::data render_args;
		render_args.set("class_name", one_class->unqualified_name());
		render_args.set("class_full_name", one_class->qualified_name());
		render_args.set("rpc_methods", generate_rpc_call_for_class(one_class));
		generator::append_output_to_stream(result, new_h_file_path.string(), rpc_call_mustache_tempalte.render(render_args));
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
	std::string file_path = "../example/generate_rpc/test_class.cpp";
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
	generator::merge_file_content(file_content, generate_rpc());
	generator::write_content_to_file(file_content);
	clang_disposeTranslationUnit(m_translationUnit);

	return 0;

}
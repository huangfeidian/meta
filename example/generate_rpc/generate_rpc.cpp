
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

json load_json_file(const std::string& filename)
{
	std::ifstream t(filename);
	std::stringstream buffer;
	buffer << t.rdbuf();
	auto cur_file_content = buffer.str();
	// std::cout<<"cur file_content is "<<cur_file_content<<std::endl;
	if (!json::accept(cur_file_content))
	{
		return {};
	}
	return json::parse(cur_file_content);
}

std::vector<std::string> get_files_in_folder(const std::string& folder)
{
	std::vector<std::string> result;
	const std::filesystem::path folder_path(folder);
	for (auto const& dir_entry : std::filesystem::directory_iterator{ folder_path })
	{
		if (!dir_entry.is_regular_file())
		{
			continue;
		}
		auto cur_path_str = dir_entry.path().string();
		if (cur_path_str.size() <= 3)
		{
			continue;
		}
		if (cur_path_str.back() != 'h' || cur_path_str[cur_path_str.size() -2 ] != '.')
		{
			continue;
		}
		result.push_back(cur_path_str);
	}
	return result;
}
std::string relative_path(const std::string& base_path, const std::string& cur_path)
{
	if (cur_path.rfind(base_path, 0) != 0)
	{
		return cur_path;
	}
	else
	{
		return cur_path.substr(base_path.size());
	}
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
		auto generated_cpp_file_name = one_class->unqualified_name() + ".generated.incpp";
		auto new_cpp_file_path = _cur_parent_path / generated_cpp_file_name;
		mustache::data render_args;
		render_args.set("class_name", one_class->unqualified_name());
		render_args.set("class_full_name", one_class->qualified_name());
		render_args.set("rpc_methods", generate_rpc_call_for_class(one_class));
		generator::append_output_to_stream(result, new_cpp_file_path.string(), rpc_call_mustache_tempalte.render(render_args));
	}
	return result;
}
int main(int argc, const char** argv)
{
	if (argc != 2)
	{
		std::cout << "please specify the json file path" << std::endl;
		return 1;
	}
	std::string json_file_path = argv[1];
	// std::string json_file_path = "../../test/config.json";
	if (json_file_path.empty())
	{
		std::cout << "empty json file path" << std::endl;
		return 1;
	}

	auto folder_iter = json_file_path.rfind('/');
	if (folder_iter == std::string::npos)
	{
		std::cout << "file path " << json_file_path << " should has / in it " << std::endl;
		return 1;
	}
	std::string file_folder = json_file_path.substr(0, folder_iter + 1);
	auto cur_json_content = load_json_file(json_file_path);

	std::vector<std::string> include_dirs;
	std::vector<std::string> compile_definitions;
	std::string src_file;
	std::string filter_namespace;
	std::string mustache_folder;
	std::string generated_folder;
	try
	{
		cur_json_content.at("include_dirs").get_to(include_dirs);

		cur_json_content.at("definitions").get_to(compile_definitions);

		cur_json_content.at("src_file").get_to(src_file);

		cur_json_content.at("namespace").get_to(filter_namespace);

		cur_json_content.at("mustache_folder").get_to(mustache_folder);

		cur_json_content.at("generated_folder").get_to(generated_folder);
	}
	catch (std::exception& e)
	{
		std::cout << "fail to parse json file error is " << e.what() << std::endl;
		return 1;
	}
	if (src_file.empty())
	{
		std::cout << "empty src file" << std::endl;
		return 1;
	}
	src_file = file_folder + src_file;
	if (mustache_folder.empty())
	{
		std::cout << "mustache folder  is empty" << std::endl;
		return 1;
	}
	if (mustache_folder[0] != '.')
	{
		std::cout << "mustache folder  " << mustache_folder << " should begin with ." << std::endl;
		return 1;
	}
	mustache_folder = file_folder + mustache_folder;

	if (generated_folder.empty())
	{
		std::cout << "generated_folder  is empty" << std::endl;
		return 1;
	}
	if (generated_folder[0] != '.')
	{
		std::cout << "generated_folder  " << generated_folder << " should begin with ." << std::endl;
		return 1;
	}
	generated_folder = file_folder + generated_folder;

	for (auto& one_include_dir : include_dirs)
	{
		if (one_include_dir.empty())
		{
			continue;
		}
		if (one_include_dir[0] == '.')
		{
			one_include_dir = file_folder + one_include_dir;
		}

	}
	auto& the_logger = utils::get_logger();
	CXIndex m_index;
	CXTranslationUnit m_translationUnit;
	std::vector<std::string> arguments;

	for (const auto& one_include_dir : include_dirs)
	{
		arguments.push_back("-I" + one_include_dir);
	}
	for (const auto& one_definition : compile_definitions)
	{
		arguments.push_back(one_definition);
	}
	arguments.push_back("-x");
	arguments.push_back("c++");
	arguments.push_back("-std=c++17");
	arguments.push_back("-D__meta_parse__");

	std::vector<const char*> cstr_arguments;

	for (const auto& i : arguments)
	{
		cstr_arguments.push_back(i.c_str());
	}


	bool display_diag = true;
	m_index = clang_createIndex(true, display_diag);
	std::string file_path = src_file;
	//std::string file_path = "sima.cpp";
	m_translationUnit = clang_createTranslationUnitFromSourceFile(m_index, file_path.c_str(), static_cast<int>(cstr_arguments.size()), cstr_arguments.data(), 0, nullptr);
	auto cursor = clang_getTranslationUnitCursor(m_translationUnit);
	the_logger.info("the root cursor is {}", utils::to_string(cursor));
	auto& cur_type_db = language::type_db::instance();
	cur_type_db.create_from_translate_unit(cursor);
	//recursive_print_decl_under_namespace("A");
	cur_type_db.build_class_under_namespace("std");
	cur_type_db.build_class_under_namespace("json");
	cur_type_db.build_class_under_namespace(filter_namespace);
	//recursive_print_func_under_namespace("A");
	//recursive_print_class_under_namespace("A");
	auto cur_namespace_classes = language::type_db::instance().get_class_with_pred([filter_namespace](const language::class_node& temp_class)
		{

			return temp_class.qualified_name().rfind(filter_namespace, 0) == 0;
		});
	json result;
	for (const auto& one_class : cur_namespace_classes)
	{
		result.push_back(json(*one_class));
	}
	ofstream json_out("type_info.json");
	json_out << setw(4) << result << endl;
	std::unordered_map<std::string, std::string> file_content;
	//utils::merge_file_content(file_content, generate_encode_decode());
	generator::merge_file_content(file_content, generate_rpc());
	generator::write_content_to_file(file_content);
	clang_disposeTranslationUnit(m_translationUnit);

	return 0;

}
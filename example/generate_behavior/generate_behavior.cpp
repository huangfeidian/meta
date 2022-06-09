
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

struct func_doc
{
	std::string brief_info;
	std::string return_info;
	std::unordered_map<std::string, std::string> param_info;
};
std::vector<string_view> split_lines(std::string_view text)
{
	std::size_t start = 0, end = 0;
	vector<string_view> tokens;
	char sep = '\n';
	while ((end = text.find(sep, start)) != std::string::npos)
	{
		if (end != start)
		{
			auto temp_end = end;
			// auto cur_temp_token = text.substr(start, temp_end - start);
			// while (text[temp_end] == '\t')
			// {
			// 	temp_end -= 1;
			// 	if (temp_end <= start)
			// 	{
			// 		break;
			// 	}
			// }
			tokens.push_back(text.substr(start, temp_end - start));
			
		}
		start = end + 1;
	}
	if (end != start)
	{
		tokens.push_back(text.substr(start));
	}
	return tokens;

}

string_view remove_prefix_empty(string_view text)
{
	std::string empty_tokens = " \t";
	std::size_t start = text.find_first_not_of(empty_tokens), end = 0;
	return text.substr(start);
}
std::pair<string_view, string_view> doxygen_parse_one_line(string_view one_line)
{
	std::string empty_tokens = " \t";
	one_line = remove_prefix_empty(one_line);
	if (one_line.size() <= 4 || one_line.substr(0, 4) != "//! ")
	{
		return { "", "" };
	}
	auto real_content = one_line.substr(4);

	if (real_content.empty())
	{
		return { "", "" };
	}
	if (real_content[0] != '\\')
	{
		return { "", real_content };
	}
	std::size_t start = real_content.find_first_of(empty_tokens);
	auto temp_key = real_content.substr(1, start - 1);
	if (temp_key.empty())
	{
		return { "", "" };
	}

	if (temp_key == "param")
	{

		auto pre_start = start + 1;
		start = real_content.find_first_of(empty_tokens, start + 1);
		if (start == string::npos)
		{
			return { "", "" };
		}
		temp_key = real_content.substr(1, start - 1);
		if (temp_key.empty())
		{
			return { "", "" };
		}
	}
	return { temp_key, real_content.substr(start + 1) };
}
unordered_map<string_view, vector<string_view>> parse_doxygen(vector<std::string_view> text)
{
	string_view key;
	string_view empty_key;
	vector<string_view> values;
	unordered_map<string_view, vector<string_view>> result;
	std::string empty_tokens = " \t";
	for (auto one_line : text)
	{
		auto[cur_key, cur_value] = doxygen_parse_one_line(one_line);
		if (cur_key.empty())
		{
			if (cur_value.empty())
			{
				if (!key.empty())
				{
					result[key] = values;
					key = empty_key;
					values.clear();
				}
			}
			else
			{
				values.push_back(cur_value);
			}
		}
		else
		{
			if (!key.empty())
			{
				result[key] = values;
			}
			key = cur_key;
			values.clear();
			values.push_back(cur_value);
		}
	}

	if (!key.empty())
	{
		result[key] = values;
	}
	return result;
}
func_doc parse_func_doc(const language::callable_node& cur_func)
{
	const std::string& comment = cur_func.comment();
	auto comment_lines = split_lines(comment);

	auto doxygen_comment = parse_doxygen(comment_lines);
	func_doc cur_func_doc;
	for (const auto&[cur_key, cur_value] : doxygen_comment)
	{
		if (cur_key == "brief")
		{
			cur_func_doc.brief_info = utils::join(cur_value, "");
			continue;
		}
		if (cur_key == "return")
		{
			cur_func_doc.return_info = utils::join(cur_value, "");
			continue;
		}
		if (cur_key.find("param") == 0)
		{
			cur_func_doc.param_info[std::string(remove_prefix_empty(cur_key.substr(5)))] = utils::join(cur_value, "");
		}
	}
	
	return cur_func_doc;

}

std::unordered_map<std::string, std::string> generate_behavior_actions()
{
	auto& the_logger = utils::get_logger();
	std::unordered_map<std::string, std::string> _annotation_value = { };
	auto all_agent_class = language::type_db::instance().get_class_with_pred([&_annotation_value](const language::class_node& _cur_node)
		{
			return language::filter_with_annotation_value<language::class_node>("behavior", _annotation_value, _cur_node);
		});
	std::unordered_map<std::string, std::string> result;
	auto behavior_action_decl_mustache = generator::load_mustache_from_file("./mustache/behavior_action_decl.mustache");
	auto behavior_action_impl_mustache = generator::load_mustache_from_file("./mustache/behavior_action_impl.mustache");
	auto static_constructor_decl_mustache = generator::load_mustache_from_file("./mustache/static_constructor_decl.mustache");
	auto static_constructor_impl_mustache = generator::load_mustache_from_file("./mustache/static_constructor_impl.mustache");
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
void dump_behavior_actions()
{
	auto& the_logger = utils::get_logger();
	json agent_actions;
	std::unordered_map<std::string, std::string> _annotation_value = { };
	auto all_agent_class = language::type_db::instance().get_class_with_pred([&_annotation_value](const language::class_node& _cur_node)
		{
			return language::filter_with_annotation_value<language::class_node>("behavior", _annotation_value, _cur_node);
		});
	auto func_pred = [](const callable_node& _cur_node)
	{
		return filter_with_annotation<callable_node>("behavior_action", _cur_node);
	};

	for (auto one_class : all_agent_class)
	{
		json::array_t cur_class_actions;

		for (auto one_func : one_class->query_method_with_pred(func_pred))
		{
			auto cur_func_doc = parse_func_doc(*one_func);
			the_logger.debug("function {} has parames:", one_func->func_name());
			for (const auto& one_arg_comment : cur_func_doc.param_info)
			{
				the_logger.debug("arg {} has comment {}", one_arg_comment.first, one_arg_comment.second);
			}
			json cur_func_info;
			cur_func_info["name"] = one_func->func_name();
			cur_func_info["brief"] = cur_func_doc.brief_info;
			cur_func_info["return"] = cur_func_doc.return_info;
			json::array_t args_info;
			for (const auto& one_arg : one_func->args_type())
			{
				json one_arg_info;
				one_arg_info["name"] = one_arg->unqualified_name();
				one_arg_info["type"] = one_arg->decl_type()->pretty_name();
				auto comment_iter = cur_func_doc.param_info.find(one_arg->unqualified_name());
				if (comment_iter != cur_func_doc.param_info.end())
				{
					one_arg_info["comment"] = comment_iter->second;
				}
				else
				{
					one_arg_info["comment"] = "";
				}
				args_info.push_back(one_arg_info);
			}
			cur_func_info["args"] = args_info;
			cur_class_actions.push_back(cur_func_info);
		}
		agent_actions[one_class->unqualified_name()] = cur_class_actions;
	}
	std::ofstream action_file = std::ofstream("actions.json");
	action_file << agent_actions.dump(1, '\t', true)<<std::endl;
	action_file.close();

}
int main(int argc, const char** argv)
{
	auto& the_logger = utils::get_logger();
	std::string file_path = argv[1];
	CXIndex m_index;
	CXTranslationUnit m_translationUnit;
	std::vector<std::string> arguments;
	arguments.push_back("-x");
	arguments.push_back("c++");
	arguments.push_back("-std=c++17");
	arguments.push_back("-D__meta_parse__");
	arguments.push_back("-fparse-all-comments");
	for (int i = 2; i < argc; i++)
	{
		arguments.push_back(argv[i]);
	}
	std::vector<const char *> cstr_arguments;

	for (const auto& i : arguments)
	{
		cstr_arguments.push_back(i.c_str());
	}

	bool display_diag = true;
	m_index = clang_createIndex(true, display_diag);
	
	//std::string file_path = "sima.cpp";
	m_translationUnit = clang_createTranslationUnitFromSourceFile(m_index, file_path.c_str(), static_cast<int>(cstr_arguments.size()), cstr_arguments.data(), 0, nullptr);
	auto cursor = clang_getTranslationUnitCursor(m_translationUnit);
	the_logger.info("the root cursor is {}", utils::to_string(cursor));
	auto& cur_type_db = language::type_db::instance();
	cur_type_db.create_from_translate_unit(cursor);
	//recursive_print_decl_under_namespace("A");
	cur_type_db.build_class_under_namespace("std");
	cur_type_db.build_class_under_namespace("spiritsaway::behavior_tree");
	//recursive_print_func_under_namespace("A");
	//recursive_print_class_under_namespace("A");
	json result = language::type_db::instance().to_json();
	ofstream json_out("type_info.json");
	json_out << setw(4) << result << endl;
	std::unordered_map<std::string, std::string> file_content;
	//utils::merge_file_content(file_content, generate_encode_decode());
	generator::merge_file_content(file_content, generate_behavior_actions());
	generator::write_content_to_file(file_content);
	dump_behavior_actions();
	clang_disposeTranslationUnit(m_translationUnit);

	return 0;

}
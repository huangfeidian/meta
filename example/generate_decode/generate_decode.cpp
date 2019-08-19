
#include <iostream>
#include <iomanip>

#include <cstdint>
#include <fstream>
#include <iomanip>

#include <iostream>
#include <sstream>
#include <queue>
#include <filesystem>
#include "nodes/type_info.h"
#include "name_space.h"
#include "utils.h"
#include "nodes/class.h"
#include "nodes/enum.h"
#include "nodes/variable.h"
#include <serialize/decode.h>
#include <utility/generate_utils.h>

#include <mustache.hpp>

using namespace std;
using namespace meta;
namespace mustache = kainjow::mustache;

template <typename T>
bool filter_with_annotation_value(const std::string& _annotation_name, const std::vector<std::string>& _annotation_value, const T& _cur_node)
{

	auto& _cur_annotations = _cur_node.annotations();
	auto cur_iter = _cur_annotations.find(_annotation_name);
	if (cur_iter == _cur_annotations.end())
	{
		return false;
	}
	if (cur_iter->second != _annotation_value)
	{
		return false;
	}
	return true;
}
template <typename T>
bool filter_with_annotation(const std::string& _annotation_name, const T& _cur_node)
{

	auto& _cur_annotations = _cur_node.annotations();
	auto cur_iter = _cur_annotations.find(_annotation_name);
	if (cur_iter == _cur_annotations.end())
	{
		return false;
	}
	return true;

}

std::string generate_decode_func_for_class(const language::class_node* one_class, mustache::mustache& decode_template)
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
			if (filter_with_annotation<language::class_node>("encode", *cur_related_class))
			{
				return true;
			}
			return false;
		});
	std::sort(_bases.begin(), _bases.end(), [](const language::type_info* a, const language::type_info* b)
		{
			if (a->name() < b->name())
			{
				return true;
			}
			else
			{
				return false;
			}
		});
	std::vector<std::string> field_decode_value = {};
	auto decode_fields = one_class->query_fields_with_pred([&field_decode_value](const language::variable_node& _cur_node)
		{
			return filter_with_annotation_value<language::variable_node>("decode", field_decode_value, _cur_node);
		});
	std::sort(decode_fields.begin(), decode_fields.end(), [](const language::variable_node* a, const language::variable_node* b)
		{
			if (a->unqualified_name() < b->unqualified_name())
			{
				return true;
			}
			else
			{
				return false;
			}

		});

	mustache::data base_list{ mustache::data::type::list };
	std::size_t decode_idx = 0;
	for (auto one_base : _bases)
	{
		auto _one_class = one_base->related_class();

		if (filter_with_annotation<language::class_node>("decode", *_one_class))
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
		if (filter_with_annotation<language::variable_node>("decode", *one_field))
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
	auto decode_str = decode_template.render(render_args);
	return decode_str;
}

std::string generate_encode_func_for_class(const language::class_node* one_class, mustache::mustache& mustache_template)
{
	// 首先encode父类 按照父类的名称排序
	auto _bases = one_class->bases();
	std::sort(_bases.begin(), _bases.end(), [](const language::type_info* a, const language::type_info* b)
		{
			if (a->name() < b->name())
			{
				return true;
			}
			else
			{
				return false;
			}
		});
	mustache::data base_list{ mustache::data::type::list };
	for (auto one_base : _bases)
	{
		base_list << mustache::data{ "base_type", one_base->name() };
	}
	// 然后encode自己的变量
	std::vector<std::string> field_encode_value = {};
	auto encode_fields = one_class->query_fields_with_pred([&field_encode_value](const language::variable_node& _cur_node)
		{
			return filter_with_annotation_value<language::variable_node>("encode", field_encode_value, _cur_node);
		});
	std::sort(encode_fields.begin(), encode_fields.end(), [](const language::variable_node* a, const language::variable_node* b)
		{
			if (a->unqualified_name() < b->unqualified_name())
			{
				return true;
			}
			else
			{
				return false;
			}
		});
	mustache::data field_list{ mustache::data::type::list };
	for (auto one_field : encode_fields)
	{
		field_list << mustache::data{ "field_name", one_field->unqualified_name() };
	}
	mustache::data render_args;
	render_args.set("fields", field_list);
	render_args.set("bases", base_list);
	auto encode_str = mustache_template.render(render_args);
	return encode_str;
}
std::unordered_map<std::string, std::string> generate_encode_decode()
{
	// 遍历所有的class 对于里面表明了需要生成decode的类进行处理
	auto& the_logger = utils::get_logger();
	std::vector<std::string> _annotation_value = { "auto" };
	auto all_decode_classes = language::type_db::instance().get_class_with_pred([&_annotation_value](const language::class_node& _cur_node)
		{
			return filter_with_annotation_value<language::class_node>("decode", _annotation_value, _cur_node);
		});
	auto all_encode_classses = language::type_db::instance().get_class_with_pred([&_annotation_value](const language::class_node& _cur_node)
		{
			return filter_with_annotation_value<language::class_node>("encode", _annotation_value, _cur_node);
		});
	std::unordered_set<const language::class_node*> all_related_classes;
	std::copy(all_encode_classses.begin(), all_encode_classses.end(), std::inserter(all_related_classes, all_related_classes.end()));
	std::copy(all_decode_classes.begin(), all_decode_classes.end(), std::inserter(all_related_classes, all_related_classes.end()));
	std::unordered_map<std::string, std::string> result;
	auto encode_func_mustache_file = std::ifstream("../mustache/encode_func.mustache");
	std::string template_str = std::string(std::istreambuf_iterator<char>(encode_func_mustache_file), std::istreambuf_iterator<char>());
	mustache::mustache encode_func_mustache_tempalte(template_str);
	auto decode_func_mustache_file = std::ifstream("../mustache/decode_func.mustache");
	template_str = std::string(std::istreambuf_iterator<char>(decode_func_mustache_file), std::istreambuf_iterator<char>());
	mustache::mustache decode_func_mustache_tempalte(template_str);
	for (auto one_class : all_related_classes)
	{
		auto cur_file_path_str = one_class->file();
		the_logger.info("get class {} with annotation prop decode location {}", one_class->unqualified_name(), cur_file_path_str);
		std::filesystem::path file_path(cur_file_path_str);
		auto _cur_parent_path = file_path.parent_path();
		auto generated_h_file_name = one_class->unqualified_name() + "_generated.h";
		auto new_h_file_path = _cur_parent_path / generated_h_file_name;

		std::ostringstream h_file_stream;
		std::ostringstream cpp_file_stream;
		cpp_file_stream << "#include " << file_path.filename() << "\n";
		if (filter_with_annotation_value<language::class_node>("encode", _annotation_value, *one_class))
		{
			auto encode_func_str = generate_encode_func_for_class(one_class, encode_func_mustache_tempalte);
			utils::append_output_to_stream(result, new_h_file_path.string(), encode_func_str);
		}
		if (filter_with_annotation_value<language::class_node>("decode", _annotation_value, *one_class))
		{
			auto decode_func_str = generate_decode_func_for_class(one_class, decode_func_mustache_tempalte);
			utils::append_output_to_stream(result, new_h_file_path.string(), decode_func_str);
		}
		
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
	std::string file_path = "../example/generate_decode/test_class.cpp";
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
	utils::merge_file_content(file_content, generate_encode_decode());
	utils::write_content_to_file(file_content);
	clang_disposeTranslationUnit(m_translationUnit);

	return 0;

}
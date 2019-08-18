
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

using namespace std;
using namespace meta;


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

std::string generate_decode_func_for_class(const language::class_node* one_class)
{
	std::ostringstream cpp_file_stream;
	cpp_file_stream << "bool " << one_class->name() << "::decode(const json& data) \n{\n";
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

	// check array size match
	cpp_file_stream << "\tif(!data.is_array())\n\t{\n\t\treturn false;\n\t}\n";
	cpp_file_stream << "\tif(data.size() !=" << _bases.size() + decode_fields.size() << ")\n\t{\n\t\treturn false;\n\t}\n";
	cpp_file_stream << "\t//begin base decode\n";
	std::size_t decode_idx = 0;
	for (auto one_base : _bases)
	{
		auto _one_class = one_base->related_class();

		if (filter_with_annotation<language::class_node>("decode", *_one_class))
		{

			// 默认encode 与decode 的需求格式统一

			cpp_file_stream << "\tif(!decode(data[" << decode_idx << "], static_cast<" << _one_class->name() << "&>(*this))\n\t{\n";
			cpp_file_stream << "\t\treturn false;\n\t}\n";
		}

		decode_idx += 1;
	}
	// 然后decode自己的变量
	cpp_file_stream << "\t//begin variable decode\n";

	for (auto one_field : decode_fields)
	{
		if (filter_with_annotation<language::variable_node>("decode", *one_field))
		{
			cpp_file_stream << "\tif(!decode(data[" << decode_idx << "], " << one_field->unqualified_name() << ")\n\t{\n";
			cpp_file_stream << "\t\treturn false;\n\t}\n";
		}
		decode_idx += 1;
	}
	cpp_file_stream << "\treturn true;\n}" << std::endl;
	return cpp_file_stream.str();
}

std::string generate_encode_func_for_class(const language::class_node* one_class)
{
	std::ostringstream cpp_file_stream;
	cpp_file_stream << "json " << one_class->name() << "::encode() const\n{\n\tjson result = json::array();\n";
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
	cpp_file_stream << "\t//begin base encode\n";
	for (auto one_base : _bases)
	{
		cpp_file_stream << "\tresult.push_back(encode(static_cast<const " << one_base->name() << "&>(*this))\n";
	}
	// 然后encode自己的变量
	std::vector<std::string> field_decode_value = {};
	auto encode_fields = one_class->query_fields_with_pred([&field_decode_value](const language::variable_node& _cur_node)
		{
			return filter_with_annotation_value<language::variable_node>("decode", field_decode_value, _cur_node);
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
	for (auto one_field : encode_fields)
	{
		cpp_file_stream << "\tresult.push_back(encode(" << one_field->unqualified_name() << ");\n";
	}
	cpp_file_stream << "\treturn result;\n}" << std::endl;
	return cpp_file_stream.str();
}
std::pair<std::string, std::string> generate_property_func_for_class(const language::class_node* one_class)
{
	// 生成一个类的所有property信息
	std::vector<std::string> property_annotate_value;
	auto property_fields = one_class->query_fields_with_pred([&property_annotate_value](const language::variable_node& _cur_node)
		{
			return filter_with_annotation_value<language::variable_node>("property", property_annotate_value, _cur_node);
		});
	std::sort(property_fields.begin(), property_fields.end(), [](const language::variable_node* a, const language::variable_node* b)
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

	std::ostringstream h_stream;
	std::ostringstream cpp_stream;
	std::string cur_class_name = one_class->qualified_name();
	
	for (auto one_field : property_fields)
	{
		auto cur_field_name = one_field->unqualified_name();
		auto cur_field_type_name = one_field->decl_type()->name();
		//ref
		h_stream << "const decltype(" << cur_field_name << ")& " << cur_field_name << "_ref() const\n";
		h_stream << "{\n\treturn " << cur_field_name << ";\n}\n";
		//mut
		h_stream << "decltype(" << cur_field_name << ")& " << cur_field_name << "_mut()\n";
		h_stream << "{\n\treturn " << cur_field_name << ";\n}\n";
		//set
		h_stream << "mutate_msg " << cur_field_name << "_set(const decltype(" << cur_field_name << ")& _value)\n";
		h_stream << "{\n\t" << cur_field_name << " = _value;\n";
		h_stream << "\t return std::make_tuple(_cur_depth, index_for_" << cur_field_name;
		h_stream << ", var_mutate_cmd::set, encode(" << cur_field_name << "));\n}\n";
		//replay_set
		h_stream << "bool Z_replay_" << cur_field_name << "_set(const json& _value)\n";
		h_stream << "{\n\treturn decode(_value, " << cur_field_name << ");\n}\n";
		//clear
		h_stream << "mutate_msg " << cur_field_name << "_clear()\n{\n";
		if (cur_field_type_name.find('<') != std::string::npos)
		{
			// is a template just use clear
			h_stream << "\t" << cur_field_name << ".clear();\n";
		}
		else
		{
			h_stream << "\t" << cur_field_name << " = 0;\n";
		}
		h_stream << "\t return std::make_tuple(_cur_depth, index_for_" << cur_field_name;
		h_stream << ", var_mutate_cmd::clear, json::array());\n}\n";
		//replay clear
		h_stream << "bool Z_replay_" << cur_field_name << "_clear(const json& _value)\n{\n";
		if (cur_field_type_name.find('<') != std::string::npos)
		{
			// is a template just use clear
			h_stream << "\t" << cur_field_name << ".clear();\n";
		}
		else
		{
			h_stream << "\t" << cur_field_name << " = 0;\n";
		}
		h_stream << "\t return true;\n\}\n";


		if (cur_field_type_name._Starts_with("std::vector<"))
		{
			// this is a vector has push_back pop_back delete_idx mutate_idx
			//push back
			h_stream << "mutate_msg " << cur_field_name << "_push_back(const decltype(" << cur_field_name << ")::value_type& _value)\n{\n";
			h_stream << "\t" << cur_field_name << ".push_back(_value);\n";
			h_stream << "\t return std::make_tuple(_cur_depth, index_for_" << cur_field_name;
			h_stream << ", var_mutate_cmd::vetor_push_back, encode_multi(_value));\n}\n";
			//replay push back
			h_stream << "bool Z_replay_" << cur_field_name << "_push_back(const json& _value)\n{\n";
			h_stream << "decltype(" << cur_field_name << ")::value_type temp;\n";
			h_stream << "\tif(decode_multi(_value, temp)\n{\n";
			h_stream << "\t\t" << cur_field_name << ".push_back(temp);\n\t\treturn true;\n\t}";
			h_stream << "\telse\n{\n\t\treturn false;\n\t}\n}";
			//pop back
			h_stream << "mutate_msg " << cur_field_name << "_pop_back()\n{\n";
			h_stream << "if("<<cur_field_name<<".size())\n\t\t" << cur_field_name << ".pop_back();\n\t}\n";
			h_stream << "\treturn std::make_tuple(_cur_depth, index_for_" << cur_field_name;
			h_stream << ", var_mutate_cmd::vector_pop_back, json::array();\n}\n";
			//replay pop back
			h_stream << "bool Z_replay_" << cur_field_name << "_pop_back(const json& _value)\n{\n";
			h_stream << "if(" << cur_field_name << ".size())\n\t\t" << cur_field_name << ".pop_back();\n\t}\n";
			h_stream << "\treturn true;\n}\n";

			// idx_mutate
			h_stream << "mutate_msg " << cur_field_name << "_idx_mutate(std::size_t idx, const decltype(" << cur_field_name << ")::value_type& _value)\n{\n";
			h_stream << "if(" << cur_field_name << ".size() > idx)\n{\n";
			h_stream << "\t\t" << cur_field_name << "[idx] = _value;\n\t}\n";
			h_stream << "\treturn std::make_tuple(_cur_depth, index_for_" << cur_field_name;
			h_stream << ", var_mutate_cmd::vector_idx_mutate, encode_multi(idx, _value);\n}\n";
			//replay idx_mutate
			h_stream << "bool Z_replay_" << cur_field_name << "_idx_mutate(const json& _value)\n{\n";
			h_stream << "\tstd::size_t idx;\n\tdecltype(" << cur_field_name << ")::value_type temp;\n";
			h_stream << "\tif(decode_multi(_value, idx, temp))\n\t{\n";
			h_stream << "\t\tif(idx >= " << cur_field_name << ".size())\n\t\t{\n";
			h_stream << "\t\t\treturn false;\n\t\t}\n";
			h_stream << "\t\telse\n\t\t{\n\t\t\t" << cur_field_name << "[idx] = temp;\n\t\t\treturn true;\n\t\t}\n\t}\n";
			h_stream << "\telse\n{\n\treturn false;\n\t}\n}";
			// idx_delete
			h_stream << "mutate_msg " << cur_field_name << "_idx_delete(std::size_t idx)\n{\n";
			h_stream << "if(" << cur_field_name << ".size() > idx)\n{\n";
			h_stream << "\t\t" << cur_field_name << ".erase("<<cur_field_name<<".begin() + idx);\n\t}\n";
			h_stream << "\treturn std::make_tuple(_cur_depth, index_for_" << cur_field_name;
			h_stream << ", var_mutate_cmd::vector_idx_delete, encode_multi(idx);\n}\n";
			// replay idx delete

			h_stream << "bool Z_replay_" << cur_field_name << "_idx_delete(const json& _value)\n{\n";
			h_stream << "\tstd::size_t idx;\n";
			h_stream << "\tif(decode_multi(_value, idx))\n\t{\n";
			h_stream << "\t\tif(idx >= " << cur_field_name << ".size())\n\t\t{\n";
			h_stream << "\t\t\treturn false;\n\t\t}\n";
			h_stream << "\t\telse\n\t\t{\n\t\t\t" << cur_field_name << ".erase("<<cur_field_name<<".begin() + idx);\n\t\t\treturn true;\n\t\t}\n\t}\n";
			h_stream << "\telse\n{\n\treturn false;\n\t}\n}";
		}
	}

	// 生成每个变量在当前类里面的偏移
	std::size_t cur_var_idx = 0;
	h_stream << "private:";
	for (auto one_field : property_fields)
	{
		h_stream << "const static var_idx_type index_for_" << one_field->unqualified_name() << " = " << cur_var_idx << ";\n";
		cur_var_idx++;
	}
	return std::make_pair(h_stream.str(), cpp_stream.str());

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
	for (auto one_class : all_related_classes)
	{
		auto cur_file_path_str = one_class->file();
		the_logger.info("get class {} with annotation prop decode location {}", one_class->unqualified_name(), cur_file_path_str);
		std::filesystem::path file_path(cur_file_path_str);
		auto _cur_parent_path = file_path.parent_path();
		auto generated_h_file_name = one_class->unqualified_name() + "_generated.h";
		auto generated_cpp_file_name = one_class->unqualified_name() + "_generated.cpp";
		auto new_h_file_path = _cur_parent_path / generated_h_file_name;
		auto new_cpp_file_path = _cur_parent_path / generated_cpp_file_name;
		the_logger.info("generate h file {} cpp file {} for class {}", new_h_file_path.string(), new_cpp_file_path.string(), one_class->unqualified_name());
		std::ostringstream h_file_stream;
		std::ostringstream cpp_file_stream;
		cpp_file_stream << "#include " << file_path.filename() << "\n";
		if (filter_with_annotation_value<language::class_node>("encode", _annotation_value, *one_class))
		{
			h_file_stream << "json encode() const;\n";
			cpp_file_stream << generate_encode_func_for_class(one_class);
		}
		if (filter_with_annotation_value<language::class_node>("decode", _annotation_value, *one_class))
		{
			h_file_stream << "bool decode(const json& data);\n";
			cpp_file_stream << generate_decode_func_for_class(one_class);
		}

		utils::append_output_to_stream(result, new_h_file_path.string(), h_file_stream.str());
		utils::append_output_to_stream(result, new_cpp_file_path.string(), cpp_file_stream.str());

	}
	return result;

}
std::unordered_map<std::string, std::string> generate_property()
{
	auto& the_logger = utils::get_logger();
	std::vector<std::string> _annotation_value = { "auto" };
	auto all_property_classes = language::type_db::instance().get_class_with_pred([&_annotation_value](const language::class_node& _cur_node)
		{
			return filter_with_annotation_value<language::class_node>("property", _annotation_value, _cur_node);
		});
	std::unordered_map<std::string, std::string> result;
	for (auto one_class : all_property_classes)
	{
		auto cur_file_path_str = one_class->file();
		std::filesystem::path file_path(cur_file_path_str);
		auto _cur_parent_path = file_path.parent_path();
		auto generated_h_file_name = one_class->unqualified_name() + "_generated.h";
		auto generated_cpp_file_name = one_class->unqualified_name() + "_generated.cpp";
		auto new_h_file_path = _cur_parent_path / generated_h_file_name;
		auto new_cpp_file_path = _cur_parent_path / generated_cpp_file_name;
		const auto&[h_content, cpp_content] = generate_property_func_for_class(one_class);
		utils::append_output_to_stream(result, new_h_file_path.string(), h_content);
		utils::append_output_to_stream(result, new_cpp_file_path.string(), cpp_content);
	}
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
	std::string file_path = "../example/generate_property/generate_property.cpp";
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
	utils::merge_file_content(file_content, generate_property());
	utils::write_content_to_file(file_content);
	clang_disposeTranslationUnit(m_translationUnit);

	return 0;

}
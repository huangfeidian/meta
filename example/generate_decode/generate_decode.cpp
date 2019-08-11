
#include <iostream>
#include <iomanip>

#include <cstdint>
#include <fstream>
#include <iomanip>

#include <iostream>
#include <queue>
#include <filesystem>
#include "nodes/type_info.h"
#include "name_space.h"
#include "utils.h"
#include "nodes/class.h"
#include "nodes/enum.h"
#include "nodes/variable.h"
#include <serialize/decode.h>

using namespace std;
using namespace meta;
bool interested_kind(CXCursorKind _cur_kind)
{
	switch (_cur_kind)
	{
	case CXCursor_ClassDecl:
	case CXCursor_StructDecl:
	case CXCursor_EnumDecl:
	case CXCursor_FunctionDecl:
	case CXCursor_Namespace:
	case CXCursor_VarDecl:
	case CXCursor_FunctionTemplate:
	case CXCursor_CXXMethod:
	case CXCursor_Constructor:
	case CXCursor_ParmDecl:
	case CXCursor_TemplateTypeParameter:
	case CXCursor_NonTypeTemplateParameter:
	case CXCursor_TemplateTemplateParameter:
	case CXCursor_ClassTemplate:
	case CXCursor_TypeAliasDecl:
	case CXCursor_TypedefDecl:
		return true;
	default:
		return false;
	}
}
void recursive_build_class_node_under_namespace(const std::string& ns_name)
{
	std::queue<language::node*> tasks;
	auto& all_ns_nodes = language::name_space::get_synonymous_name_spaces(ns_name);
	auto & the_logger = utils::get_logger();
	auto cur_visitor = [&ns_name, &the_logger](const language::node* _node)
	{
		switch (_node->get_kind())
		{
		case CXCursor_ClassTemplate:
		case CXCursor_ClassDecl:
		case CXCursor_StructDecl:
		{
			if (clang_isCursorDefinition(_node->get_cursor()))
			{
				auto temp_node = new language::class_node(_node);
				the_logger.info("new class {}", temp_node->to_json().dump(4));
				break;
			}
			else
			{
				the_logger.info("pre decl for class {}", _node->get_name());
				break;
			}

		}
		case CXCursor_EnumDecl:
		{
			auto temp_node = new language::enum_node(_node);
			the_logger.info("new enum {}", temp_node->to_json().dump(4));
			break;
		}
		case CXCursor_TypedefDecl:
		case CXCursor_TypeAliasDecl:
		{
			language::type_db::instance().get_alias_typedef(_node->get_cursor());
			break;
		}
		default:
			break;
		}

		return language::node_visit_result::visit_recurse;

	};
	for (const auto& i : all_ns_nodes)
	{
		language::bfs_visit_nodes(i, cur_visitor);
	}

}

void generate_decode()
{
	// 遍历所有的class 对于里面表明了需要生成decode的类进行处理
	auto& the_logger = utils::get_logger();
	auto _class_with_decode_prop = [&the_logger](const language::class_node& _cur_node)
	{
		auto& _cur_annotations = _cur_node.annotations();
		auto cur_iter = _cur_annotations.find("decode");
		if (cur_iter == _cur_annotations.end())
		{
			return false;
		}
		else
		{
			switch (cur_iter->second.size())
			{
			case 0:
				the_logger.error("class {} has decode annotation but without args", _cur_node.name());
				return false;
				break;
			case 1:
				if (cur_iter->second[0] == "auto")
				{
					return true;
				}
				else
				{
					return false;
				}
			default:
				return false;
				break;
			}
		}
	};
	auto _field_with_decode_prop = [&the_logger](const language::variable_node& _cur_node)
	{
		auto& _cur_annotations = _cur_node.annotations();
		auto cur_iter = _cur_annotations.find("decode");
		if (cur_iter == _cur_annotations.end())
		{
			return false;
		}
		else
		{
			return true;
		}
	};
	auto& all_decode_classes = language::type_db::instance().get_class_with_pred(_class_with_decode_prop);
	for (auto one_class : all_decode_classes)
	{
		auto cur_file_path_str = one_class->file();
		the_logger.info("get class {} with annotation prop decode location {}", one_class->unqualified_name(), cur_file_path_str);
		std::filesystem::path file_path(cur_file_path_str);
		auto _cur_parent_path = file_path.parent_path();
		auto generated_h_file_name = one_class->unqualified_name() + "_generated.h";
		auto generated_cpp_file_name = one_class->unqualified_name() + "_generated.cpp";
		auto new_h_file_path = _cur_parent_path / generated_h_file_name;
		auto new_cpp_file_path = _cur_parent_path / generated_cpp_file_name;
		if (!std::filesystem::exists(new_h_file_path))
		{
			the_logger.error("generated file {} not exist", new_h_file_path.string());
			continue;
		}
		if (!std::filesystem::exists(new_cpp_file_path))
		{
			the_logger.error("generated file {} not exist", new_cpp_file_path.string());
			continue;
		}
		the_logger.info("generate h file {} cpp file {} for class {}", new_h_file_path.string(), new_cpp_file_path.string(), one_class->unqualified_name());
		std::ofstream h_file_stream(new_h_file_path);
		h_file_stream << "json decode() const;" << std::endl;
		h_file_stream.close();
		std::ofstream cpp_file_stream(new_cpp_file_path);
		auto decode_fields = one_class->query_fields_with_pred(_field_with_decode_prop);
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
		cpp_file_stream << "#include " << file_path.filename() << "\n";
		cpp_file_stream << "json " << one_class->name() << "::decode() const\n{\n\tjson result = json::array();\n";
		for (auto one_field : decode_fields)
		{
			cpp_file_stream << "\tresult.push_back(decode(" << one_field->unqualified_name() << ");\n";
		}
		cpp_file_stream << "\treturn result;\n}" << std::endl;
		cpp_file_stream.close();
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
	the_logger.info("arguments is {}", utils::join(arguments, ","));
	std::vector<const char *> cstr_arguments;

	for (const auto& i : arguments)
	{
		cstr_arguments.push_back(i.c_str());
	}

	bool display_diag = true;
	m_index = clang_createIndex(true, display_diag);
	std::string file_path = "../example/decode/test_class.cpp";
	//std::string file_path = "sima.cpp";
	m_translationUnit = clang_createTranslationUnitFromSourceFile(m_index, file_path.c_str(), static_cast<int>(cstr_arguments.size()), cstr_arguments.data(), 0, nullptr);
	auto cursor = clang_getTranslationUnitCursor(m_translationUnit);
	the_logger.info("the root cursor is {}", utils::to_string(cursor));
	auto visitor = [](CXCursor cur, CXCursor parent, CXClientData data)
	{
		// cout << "cur_cursor kind " << cur.kind << endl;
		if (cur.kind == CXCursor_LastPreprocessing)
			return CXChildVisit_Break;
		// if (clang_isCursorDefinition(cur) && interested_kind(cur.kind))
		if (interested_kind(cur.kind))
		{
			language::node_db::get_instance().create_node(cur);
		}
		return CXChildVisit_Recurse;
	};
	clang_visitChildren(cursor, visitor, nullptr);
	//recursive_print_decl_under_namespace("A");
	recursive_build_class_node_under_namespace("std");
	recursive_build_class_node_under_namespace("test");
	//recursive_print_func_under_namespace("A");
	//recursive_print_class_under_namespace("A");
	json result = language::type_db::instance().to_json();
	ofstream json_out("type_info.json");
	json_out << setw(4) << result << endl;
	generate_decode();
	clang_disposeTranslationUnit(m_translationUnit);

	return 0;

}
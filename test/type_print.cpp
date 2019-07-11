
#include "name_space.h"
#include "utils.h"
#include <iostream>
#include <queue>
#include "nodes/type_info.h"
#include <fstream>
#include <iomanip>
#include "nodes/class.h"

using namespace std;
using namespace meta;
using json = nlohmann::json;
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
		return true;
	default:
		return false;
	}
}
void recursive_dump_type_info(CXType _cur_type)
{
	auto& the_logger = utils::get_logger();
	language::type_db::instance().get_type(_cur_type);
	auto is_const = clang_isConstQualifiedType(_cur_type);
	auto is_refer = _cur_type.kind == CXType_LValueReference;
	auto is_volatile = clang_isVolatileQualifiedType(_cur_type);
	auto is_pointer = clang_getPointeeType(_cur_type);
	auto is_const_ref = false;
	
	if (is_pointer.kind)
	{
		is_const_ref = clang_isConstQualifiedType(is_pointer);
		recursive_dump_type_info(is_pointer);
	}
	auto type_decl_self = clang_getTypeDeclaration(_cur_type);
	auto type_decl_ref = clang_getTypeDeclaration(is_pointer);
	the_logger.info("type {} kind {} is_const {} is_reference {} is_volatile {} is_pointer to {} is_const_ref {} self decl is {} ref decl is {}", utils::to_string(_cur_type), _cur_type.kind, is_const, is_refer, is_volatile, utils::to_string(is_pointer), is_const_ref, utils::get_qualified_name_from_cursor(type_decl_self), utils::get_qualified_name_from_cursor(type_decl_ref));
	auto tee_template_arg_num = clang_Type_getNumTemplateArguments(is_pointer);
	if (tee_template_arg_num != -1)
	{
		the_logger.info("pointee {} is template", utils::to_string(is_pointer));
		for (int i = 0; i < tee_template_arg_num; i++)
		{
			auto temp_type = clang_Type_getTemplateArgumentAsType(is_pointer, i);
			the_logger.info("pointee template arg {}  has type {}", i, utils::to_string(temp_type));
			recursive_dump_type_info(temp_type);

		}
	}
	auto template_arg_num = clang_Type_getNumTemplateArguments(_cur_type);
	if (template_arg_num != -1)
	{
		the_logger.info("{} is template", utils::to_string(_cur_type));
		for (int i = 0; i < template_arg_num; i++)
		{
			auto temp_type = clang_Type_getTemplateArgumentAsType(_cur_type, i);
			the_logger.info("template arg {}  has type {}", i, utils::to_string(temp_type));
			recursive_dump_type_info(temp_type);
		}
	}
}
void recursive_print_anything_under_cursor(CXCursor _in_cursor)
{
	auto& the_logger = utils::get_logger();

	auto visitor = [] (CXCursor cur, CXCursor parent, CXClientData data)
	{
		auto& the_logger = utils::get_logger();
		auto ref_cursor = clang_getCursorDefinition(cur);
		auto _cur_type = clang_getCursorType(cur);
		auto is_const = clang_isConstQualifiedType(_cur_type);
		auto is_refer = _cur_type.kind == CXType_LValueReference;
		auto is_volatile = clang_isVolatileQualifiedType(_cur_type);
		auto is_pointer = clang_getPointeeType(_cur_type);
		auto is_const_ref = false;
		if (is_pointer.kind)
		{
			is_const_ref = clang_isConstQualifiedType(is_pointer);
		}
		the_logger.info("parent {} meet child {} with kind {} ref name {} is_const {} is_reference {} is_volatile {} is_pointer {} is_const_ref {}", utils::to_string(parent), utils::to_string(clang_getCursorSpelling(cur)), utils::to_string(clang_getCursorKind(cur)), utils::get_qualified_name_from_cursor(ref_cursor),  is_const, is_refer, is_volatile, utils::to_string(is_pointer), is_const_ref);
		auto tee_template_arg_num = clang_Type_getNumTemplateArguments(is_pointer);
		if (tee_template_arg_num != -1)
		{
			the_logger.info("pointee {} is template", utils::to_string(is_pointer));
			for (int i = 0; i < tee_template_arg_num; i++)
			{
				the_logger.info("pointee template arg {}  has type {}", i, utils::to_string(clang_Type_getTemplateArgumentAsType(is_pointer, i)));
			}
		}
		auto template_arg_num = clang_Type_getNumTemplateArguments(_cur_type);
		if (template_arg_num != -1)
		{
			the_logger.info("{} is template", utils::to_string(_cur_type));
			for (int i = 0; i < template_arg_num; i++)
			{
				the_logger.info("template arg {}  has type {}", i, utils::to_string(clang_Type_getTemplateArgumentAsType(_cur_type, i)));
			}
		}
		return CXChildVisit_Recurse;
	};
	clang_visitChildren(_in_cursor, visitor, nullptr);
}
void recursive_print_decl_under_namespace(const std::string& ns_name)
{
	std::queue<language::node*> tasks;
	auto& all_ns_nodes = language::name_space::get_synonymous_name_spaces(ns_name);
	for (const auto& one_node : all_ns_nodes)
	{
		tasks.push(one_node);
	}
	auto cur_visitor = [&ns_name](const language::node* temp_node)
	{
		if (interested_kind(temp_node->get_kind()))
		{
			utils::get_logger().debug("node {} is {} under namespace {}", temp_node->get_qualified_name(), utils::to_string(temp_node->get_kind()), ns_name);
		}
		return language::node_visit_result::visit_recurse;

	};
	for (const auto& i : all_ns_nodes)
	{
		language::bfs_visit_nodes(i, cur_visitor);
	}
}
void print_template_type(CXType type_a)
{
	auto & the_logger = utils::get_logger();
	auto cur_type_name = utils::to_string(clang_getTypeSpelling(type_a));
	if (cur_type_name.find("vector") == std::string::npos)
	{
		return;
	}
	
	auto temp_cur = clang_getTypeDeclaration(type_a);
	the_logger.info("type {} clang_getTypeDeclaration {}", utils::to_string(clang_getTypeSpelling(type_a)), utils::to_string(clang_getCursorSpelling(temp_cur)));
	// 判断类型是否是一个模板实例化，普通的类的模板参数是-1
	int num = clang_Type_getNumTemplateArguments(type_a);
	if (num != -1)
	{
		the_logger.info("type {} is templated type with arg num {}", utils::to_string(clang_getTypeSpelling(type_a)), num);
	}
	else
	{
		return;
	}
	// 遍历全部的实例化的模板参数
	for (auto loop = 0; loop < num; ++loop) {
		CXType temp_type = clang_Type_getTemplateArgumentAsType(type_a, loop);
		the_logger.info("arg num {} has type {}", loop, utils::to_string(clang_getTypeSpelling(temp_type)));
	}

}
void print_template_func_decl_info(const language::node* _node)
{
	if (!_node)
	{
		return;
	}
	if (_node->get_kind() != CXCursor_FunctionDecl && _node->get_kind() != CXCursor_FunctionTemplate && _node->get_kind() != CXCursor_CXXMethod)
	{
		return;
	}
	auto & the_logger = utils::get_logger();
	auto cur_cursor = _node->get_cursor();
	auto ref_cursor = clang_getCursorDefinition(cur_cursor);
	//if (!clang_isCursorDefinition(cur_cursor))
	//{
	//	the_logger.info("cursor {} is a reference to cursor {}", utils::to_string(clang_getCursorSpelling(cur_cursor)), utils::to_string(clang_getCursorSpelling(ref_cursor)));
	//	return;
	//}
	
	const auto& all_children = _node->get_children_with_kind(CXCursor_ParmDecl);
	the_logger.info("func {} has {} children for parm", utils::to_string(clang_getCursorSpelling(_node->get_cursor())), all_children.size());
	for (const auto one_node : all_children)
	{
		
		auto _cur_cursor = one_node->get_cursor();
		the_logger.info("param name is {}", utils::to_string(_cur_cursor));
		auto _cur_type = clang_getCursorType(_cur_cursor);
		recursive_dump_type_info(_cur_type);
	}

}
void print_class_decl_info(const language::node* _node)
{
	if (!_node)
	{
		return;
	}
	if (_node->get_kind() != CXCursor_ClassTemplate && _node->get_kind() != CXCursor_ClassDecl && _node->get_kind() != CXCursor_StructDecl)
	{
		return;
	}
	auto & the_logger = utils::get_logger();
	auto cur_cursor = _node->get_cursor();
	recursive_print_anything_under_cursor(cur_cursor);
	auto cur_type = clang_getCursorType(cur_cursor);
	recursive_dump_type_info(cur_type);


}
void recursive_print_class_under_namespace(const std::string& ns_name)
{
	std::queue<language::node*> tasks;
	auto& all_ns_nodes = language::name_space::get_synonymous_name_spaces(ns_name);

	auto cur_visitor = [&ns_name](const language::node* temp_node)
	{
		print_class_decl_info(temp_node);

		return language::node_visit_result::visit_recurse;

	};
	for (const auto& i : all_ns_nodes)
	{
		language::bfs_visit_nodes(i, cur_visitor);
	}
	json result = language::type_db::instance().to_json();
	ofstream json_out("type_info.json");
	json_out << setw(4) << result << endl;
}
void recursive_build_class_node_under_namespace(const std::string& ns_name)
{
	std::queue<language::node*> tasks;
	auto& all_ns_nodes = language::name_space::get_synonymous_name_spaces(ns_name);
	auto & the_logger = utils::get_logger();
	auto cur_visitor = [&ns_name, &the_logger](const language::node* _node)
	{
		if (_node->get_kind() == CXCursor_ClassTemplate || _node->get_kind() != CXCursor_ClassDecl || _node->get_kind() != CXCursor_StructDecl)
		{
			auto temp_node = new language::class_node(_node);
			the_logger.info("new class {}", temp_node->to_json().dump(4));
		}
		return language::node_visit_result::visit_recurse;

	};
	for (const auto& i : all_ns_nodes)
	{
		language::bfs_visit_nodes(i, cur_visitor);
	}
}
void print_func_decl_info(const language::node* _node)
{
	if (!_node)
	{
		return;
	}
	auto _cur_cursor = _node->get_cursor();

	int num_args = clang_Cursor_getNumArguments(_cur_cursor);
	auto & the_logger = utils::get_logger();
	
	auto _cur_type = clang_getCursorType(_cur_cursor);
	auto func_name = utils::to_string(clang_getCursorSpelling(_cur_cursor));
	auto func_type_name = utils::to_string(clang_getTypeSpelling(_cur_type));
	the_logger.info("find func {} with type {} num_args {} ", func_name, func_type_name, num_args);
	if (num_args < 0)
	{
		return;
	}
	
	auto return_type = utils::to_string(clang_getTypeSpelling(clang_getResultType(_cur_type)));
	
	for (int i = 0; i < num_args; i++)
	{
		auto arg_cursor = clang_Cursor_getArgument(_cur_cursor, i);
		auto arg_name = utils::to_string(clang_getCursorSpelling(arg_cursor));
		if (arg_name.empty())
		{
			arg_name = "no_name";
		}
		auto arg_data_type = utils::to_string(clang_getTypeSpelling(clang_getArgType(_cur_type, i)));
		the_logger.info("arg idx {} has name {} type {}", i, arg_name, arg_data_type);
	}
}
void recursive_print_func_under_namespace(const std::string& ns_name)
{
	std::queue<language::node*> tasks;
	auto& all_ns_nodes = language::name_space::get_synonymous_name_spaces(ns_name);

	auto cur_visitor = [&ns_name](const language::node* temp_node)
	{
		print_template_func_decl_info(temp_node);

		return language::node_visit_result::visit_recurse;

	};
	for (const auto& i : all_ns_nodes)
	{
		language::bfs_visit_nodes(i, cur_visitor);
	}
	json result = language::type_db::instance().to_json();
	ofstream json_out("type_info.json");
	json_out << setw(4) << result << endl;
	

}
int main(int argc, char* argv[])
{
	CXIndex m_index;
	CXTranslationUnit m_translationUnit;
	std::vector<std::string> arguments;
	arguments.push_back("-x");
	arguments.push_back("c++");
	arguments.push_back("-std=c++17");

	std::vector<const char *> cstr_arguments;

	for (const auto& i : arguments)
	{
		cstr_arguments.push_back(i.c_str());
	}
	bool display_diag = true;
	m_index = clang_createIndex(true, display_diag);
	std::string file_path = "../test/test.cpp";
	m_translationUnit = clang_createTranslationUnitFromSourceFile(m_index, file_path.c_str(), static_cast<int>(cstr_arguments.size()), cstr_arguments.data(), 0, nullptr);
	auto cursor = clang_getTranslationUnitCursor(m_translationUnit);
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
	recursive_build_class_node_under_namespace("A");
	//recursive_print_func_under_namespace("A");
	clang_disposeTranslationUnit(m_translationUnit);
	return 0;
}
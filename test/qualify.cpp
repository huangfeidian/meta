#include "node.h"
#include "name_space.h"
#include "utils.h"
#include <iostream>
#include <queue>
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
		return true;
	default:
		return false;
	}
}
void recursive_print_decl_under_namespace(const std::string& ns_name)
{
	std::queue<language::node*> tasks;
	auto& all_ns_nodes = language::name_space::get_synonymous_name_spaces(ns_name);
	for (const auto& one_node : all_ns_nodes)
	{
		tasks.push(one_node);
	}
	while (!tasks.empty())
	{
		auto temp_node = tasks.front();
		tasks.pop();
		if (temp_node->get_kind() == CXCursor_ClassDecl)
		{
			utils::get_logger().debug("node {} is class decl under namespace {}", temp_node->get_qualified_name(), ns_name);
		}
		for (const auto& i : temp_node->get_all_children())
		{
			tasks.push(i);
		}

	}


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
		if (clang_isCursorDefinition(cur) && interested_kind(cur.kind))
		{
			language::node_db::get_instance().create_node(cur);
		}
		return CXChildVisit_Recurse;
	};
	clang_visitChildren(cursor, visitor, nullptr);
	recursive_print_decl_under_namespace("A");
	clang_disposeTranslationUnit(m_translationUnit);
	return 0;
}
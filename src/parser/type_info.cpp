﻿#include <meta/parser/nodes/type_info.h>
#include <meta/parser/nodes/class.h>
#include <meta/parser/nodes/enum.h>
namespace
{
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
bool is_reference_type(CXType _cur_type)
{
	return _cur_type.kind == CXType_LValueReference || _cur_type.kind == CXType_RValueReference;
}
bool is_pointer_type(CXType _cur_type)
{
	return _cur_type.kind == CXType_Pointer;
}
bool is_lvalue_refer(CXType _cur_type)
{
	return _cur_type.kind == CXType_LValueReference;
}
bool is_rvalue_refer(CXType _cur_type)
{
	return _cur_type.kind == CXType_RValueReference;
}
bool is_const_type(CXType _cur_type)
{
	return clang_isConstQualifiedType(_cur_type);
}
bool is_alias(CXType _cur_type)
{
	return _cur_type.kind == CXType_Typedef;
}

} // namespace
namespace spiritsaway::meta::language
{
type_info::type_info(const std::string &_in_name, enum CXTypeKind _in_kind) : 
	m_kind(_in_kind),
	m_ref_type(nullptr),
	m_name(_in_name)
{
}
type_info::type_info(const std::string &_in_name, CXType _in_type, const type_info *_in_ref_type) : m_type(_in_type),
																									m_kind(_in_type.kind),
																									m_ref_type(_in_ref_type),
																									m_name(_in_name)
{
}
const std::string& type_info::name() const
{
	return m_name;
}
const std::string& type_info::unqualified_name() const
{
	return m_name;
}
const std::string& type_info::qualified_name() const
{
	return m_name;
}
const std::string& type_info::pretty_name() const
{
	if (m_pretty_name.empty())
	{
		if (m_name.empty())
		{
			return m_name;
		}
		else
		{
			m_pretty_name = m_name;
			for (const auto& one_name_pair : type_db::instance().annotated_typenames())
			{
				auto replace_begin = m_pretty_name.find(one_name_pair.first);
				while (replace_begin != std::string::npos)
				{
					m_pretty_name.replace(replace_begin, one_name_pair.first.size(), one_name_pair.second);
					replace_begin = m_pretty_name.find(one_name_pair.first);
				}
			}
			auto& the_logger = utils::get_logger();
			the_logger.info("type {} set pretty name {}", m_name, m_pretty_name);
			return m_pretty_name;
		}
	}
	else
	{
		return m_pretty_name;
	}
}
CXType type_info::type() const
{
	return m_type;
}
enum CXTypeKind type_info::kind() const
{
	return m_kind;
}
const type_info *type_info::ref_type() const
{
	return m_ref_type;
}
void type_info::set_type(CXType _in_type)
{
	m_type = _in_type;
	m_kind = m_type.kind;
}
const std::vector<const type_info *> &type_info::template_args() const
{
	return m_template_args;
}
bool type_info::is_completed() const
{
	if (m_kind == CXTypeKind::CXType_Unexposed)
	{
		auto decl_type = clang_getTypeDeclaration(m_type);
		return int(decl_type.kind) != 0;
	}
	if (m_template_args.empty())
	{
		return true;
	}
	for (auto i : m_template_args)
	{
		if (!i->is_completed())
		{
			return false;
		}
	}
	return true;
}
bool type_info::is_templated() const
{
	return !m_template_args.empty();
}
bool type_info::is_template_arg() const
{
	return !utils::template_types::instance().get_type(m_type).empty();
}
bool type_info::is_callable() const
{
	return false;
}
bool type_info::is_const() const
{
	return clang_isConstQualifiedType(m_type);
}
bool type_info::is_reference() const
{
	return m_kind == CXTypeKind::CXType_LValueReference || m_kind == CXTypeKind::CXType_RValueReference;
}
bool type_info::is_alias() const
{
	return m_kind == CXTypeKind::CXType_Typedef;
}
bool type_info::is_lvalue_refer() const
{
	return m_kind == CXTypeKind::CXType_LValueReference;
}
bool type_info::is_rvalue_refer() const
{
	return m_kind == CXTypeKind::CXType_RValueReference;
}
bool type_info::is_pointer() const
{
	return m_kind == CXTypeKind::CXType_Pointer || m_kind == CXTypeKind::CXType_BlockPointer;
}
const type_info *type_info::point_to() const
{
	if (!is_pointer())
	{
		return nullptr;
	}
	return m_ref_type;
}
const type_info *type_info::refer_to() const
{
	if (!is_reference())
	{
		return nullptr;
	}
	return m_ref_type;
}
const type_info *type_info::alias_to() const
{
	if (!is_alias())
	{
		return nullptr;
	}
	return m_ref_type;
}
bool type_info::can_accept_arg_type(const type_info *arg_type) const
{
	// TODO const and non const mismatch
	if (arg_type == this)
	{
		return true;
	}
	auto alias_to_type = alias_to();
	if (alias_to_type)
	{
		return alias_to_type->can_accept_arg_type(arg_type);
	}
	auto other_alias_to_type = arg_type->alias_to();
	if (other_alias_to_type)
	{
		return can_accept_arg_type(other_alias_to_type);
	}

	auto pointer_type = point_to();
	if (pointer_type)
	{
		auto other_pointer_type = arg_type->point_to();
		if (!other_pointer_type)
		{
			return false;
		}
		return pointer_type->can_accept_arg_type(other_pointer_type);
	}
	auto other_refer_type = arg_type->refer_to();
	if (other_refer_type)
	{
		return can_accept_arg_type(other_refer_type);
	}
	auto ref_type = refer_to();
	if (ref_type)
	{

		return ref_type->can_accept_arg_type(arg_type);
	}
	if (m_related_class)
	{
		std::vector<const type_info *> _arg_vec;
		_arg_vec.push_back(arg_type);
		return m_related_class->has_constructor_for(_arg_vec);
	}
	return false;
}
bool type_info::set_related_class(class_node *_in_class)
{
	auto &the_logger = utils::get_logger();
	if (m_related_class)
	{
		return false;
	}
	if (!_in_class)
	{
		return false;
	}
	auto &qualified_class_name = _in_class->get_node()->get_qualified_name();
	if (qualified_class_name != m_name)
	{
		the_logger.info("fail to set related class with class qualified name {} type name {}", qualified_class_name, m_name);
		return false;
	}
	else
	{
		m_related_class = _in_class;
		return true;
	}
}
const class_node *type_info::related_class() const
{
	return m_related_class;
}
json type_info::to_json() const
{
	json result;
	result["name"] = m_name;
	result["kind"] = static_cast<std::uint32_t>(m_kind);
	result["kind_name"] = utils::to_string(m_kind);
	result["is_const"] = is_const();
	result["is_pointer"] = is_pointer();
	result["is_alias"] = is_alias();
	result["is_refer"] = is_reference();
	if (m_ref_type)
	{
		result["ref_type"] = m_ref_type->name();
	}
	if (!m_template_args.empty())
	{
		std::vector<std::string> args;
		for (const auto i : m_template_args)
		{
			args.push_back(i->name());
		}
		result["args"] = args;
	}
	if (m_related_class)
	{
		result["related_class"] = m_related_class->name();
	}
	return result;
}

type_info *type_db::get_type_for_const(CXType _in_type)
{
	//_in_type = clang_getCanonicalType(_in_type);
	auto &the_logger = utils::get_logger();
	auto full_name = utils::to_string(_in_type);
	the_logger.debug("get_type_for_const {}", full_name);
	auto cur_kind = _in_type.kind;
	auto decl_cursor = clang_getTypeDeclaration(_in_type);
	if (clang_Cursor_isNull(decl_cursor) || decl_cursor.kind == CXCursor_NoDeclFound)
	{
		if (cur_kind >= CXType_Void && cur_kind <= CXType_Float16)
		{
			// for const T with T be builtin types
			// for example const int
			auto non_const_name = full_name.substr(6); // "const int" -> "int"
			type_info *non_const_type;
			auto non_const_type_iter = m_type_data.find(non_const_name);

			if (non_const_type_iter == m_type_data.end())
			{
				non_const_type = new type_info(non_const_name, cur_kind);
				m_type_data[non_const_name] = non_const_type;
			}
			else
			{
				non_const_type = non_const_type_iter->second;
			}

			auto final_result = new type_info(full_name, _in_type, non_const_type);
			m_type_data[full_name] = final_result;
			return final_result;
		}
		else
		{

			//auto pointer_to_type = clang_getCanonicalType(clang_getPointeeType(_in_type));
			auto pointer_to_type = clang_getPointeeType(_in_type);
			if (pointer_to_type.kind != CXTypeKind::CXType_Invalid)
			{
				// for const T *const
				auto pointee_type = get_type(pointer_to_type); // for const T
				const auto &temp_name = pointee_type->name();
				std::string pointer_to_type_name = temp_name + " *";
				type_info *temp_result = nullptr;
				auto temp_iter = m_type_data.find(pointer_to_type_name);
				if (temp_iter == m_type_data.end())
				{
					temp_result = new type_info(pointer_to_type_name, _in_type, pointee_type);
				}
				else
				{
					temp_result = temp_iter->second;
				}
				auto final_result = new type_info(full_name, _in_type, temp_result);

				m_type_data[full_name] = final_result;
				return final_result;
			}
			else
			{
				// some error case I cant handle
				auto final_result = new type_info(full_name, _in_type, nullptr);
				m_type_data[full_name] = final_result;
				return final_result;
			}
		}
	}
	else
	{
		// some case for clang_getCursorType(clang_getTypeDeclaration(const T)) == T
		auto decl_type = clang_getCursorType(decl_cursor);
		auto decl_type_info = get_type(decl_type);
		auto final_type = new type_info(full_name, _in_type, decl_type_info);
		m_type_data[full_name] = final_type;
		return final_type;
	}
}
type_info *type_db::get_type_for_pointee(CXType _in_type)
{
	//_in_type = clang_getCanonicalType(_in_type);
	auto full_name = utils::to_string(_in_type);
	auto &the_logger = utils::get_logger();
	the_logger.debug("get_type_for_pointee {}", full_name);
	//auto pointer_to_type = clang_getCanonicalType(clang_getPointeeType(_in_type));
	auto pointer_to_type = clang_getPointeeType(_in_type);
	auto pointee_type = get_type(pointer_to_type);
	auto final_result = new type_info(full_name, _in_type, pointee_type);
	m_type_data[full_name] = final_result;
	return final_result;
}
type_info *type_db::get_type_for_template(CXType _in_type)
{
	auto full_name = utils::to_string(_in_type);
	auto &the_logger = utils::get_logger();
	the_logger.debug("get_type_for_template {}", full_name);
	auto argu_num = clang_Type_getNumTemplateArguments(_in_type);
	type_info *base_type = get_base_type_for_template(_in_type);

	std::vector<const type_info *> arg_types;
	for (int i = 0; i < argu_num; i++)
	{
		auto temp_arg_type = get_type(clang_Type_getTemplateArgumentAsType(_in_type, i));
		arg_types.push_back(temp_arg_type);
	}
	auto final_result = new type_info(full_name, _in_type, base_type);
	final_result->m_template_args = arg_types;
	m_type_data[full_name] = final_result;
	return final_result;
}
type_info *type_db::get_base_type_for_template(CXType _in_type)
{
	auto decl_cursor = clang_getTypeDeclaration(_in_type);
	auto decl_name = utils::full_name(decl_cursor);
	auto temp_str0 = utils::to_string(decl_cursor);
	auto decl_type = clang_getCursorType(decl_cursor);
	auto temp_str1 = utils::to_string(decl_type);
	auto &the_logger = utils::get_logger();
	the_logger.debug("get_base_type_for_template in_type {} decl_name {} temp_str0 {} decl_type {}", utils::to_string(_in_type), decl_name, temp_str0, temp_str1);
	auto decl_iter = m_type_data.find(decl_name);
	if (decl_iter != m_type_data.end())
	{
		return decl_iter->second;
	}

	if (clang_equalTypes(_in_type, decl_type))
	{
		return nullptr;
	}
	else
	{
		auto defi_cursor = clang_getCursorDefinition(decl_cursor);
		auto temp_str2 = utils::to_string(defi_cursor);
		auto defi_type = clang_getCursorType(defi_cursor);
		auto temp_str3 = utils::to_string(defi_type);
		the_logger.debug("get defi_cursor {} defi_type {}", temp_str2, temp_str3);
		return get_type(defi_type);
	}
	//if (base_type == nullptr)
	//{
	//	if (_in_type.kind == CXTypeKind::CXType_Unexposed)
	//	{

	//	}
	//}
}
type_info *type_db::get_type(CXType _in_type)
{
	//_in_type = clang_getCanonicalType(_in_type);
	auto &the_logger = utils::get_logger();
	auto full_name = utils::to_string(_in_type);
	the_logger.debug("get_type for type {}", full_name);
	auto type_iter = m_type_data.find(full_name);
	if (type_iter != m_type_data.end())
	{
		auto result = type_iter->second;
		if (result->m_type.kind != CXTypeKind::CXType_Invalid)
		{
			result->set_type(_in_type);
		}
		return type_iter->second;
	}
	if (_in_type.kind == CXType_Auto)
	{
		auto final_type = new type_info(full_name, _in_type, nullptr);
		m_type_data[full_name] = final_type;
		return final_type;
	}
	auto is_const = clang_isConstQualifiedType(_in_type);
	if (is_const)
	{
		return get_type_for_const(_in_type);
	}

	auto pointer_to_type = clang_getPointeeType(_in_type);

	if (pointer_to_type.kind != CXTypeKind::CXType_Invalid)
	{
		// is a reference type but not a const T* const
		// is not a reference type
		return get_type_for_pointee(_in_type);
	}
	auto argu_num = clang_Type_getNumTemplateArguments(_in_type);
	if (argu_num > 0)
	{
		return get_type_for_template(_in_type);
	}
	auto final_type = new type_info(full_name, _in_type, nullptr);
	m_type_data[full_name] = final_type;
	return final_type;
}
type_info *type_db::get_type_for_template_class(CXCursor _template_class_decl)
{
	auto &the_logger = utils::get_logger();
	auto qualified_name = utils::full_name(_template_class_decl);
	if (_template_class_decl.kind != CXCursorKind::CXCursor_ClassTemplate)
	{
		the_logger.warn("get_type_for_template_class for cursor {} with invalid kind {}", qualified_name, utils::to_string(_template_class_decl.kind));
		return nullptr;
	}
	auto cur_iter = m_type_data.find(qualified_name);
	if (cur_iter != m_type_data.end())
	{
		the_logger.warn("duplicated template class decl {}", qualified_name);
		return cur_iter->second;
	}
	the_logger.debug("create type_info for  template class decl {}", qualified_name);
	auto new_type_info = new type_info(qualified_name, CXTypeKind::CXType_Invalid);
	m_type_data[qualified_name] = new_type_info;
	return new_type_info;
}
type_info *type_db::get_alias_typedef(CXCursor _in_cursor)
{
	auto &the_logger = utils::get_logger();
	if (_in_cursor.kind != CXCursor_TypedefDecl && _in_cursor.kind != CXCursor_TypeAliasDecl)
	{

		the_logger.warn("get_alias_typedef for invalid cursor {}", utils::to_string(_in_cursor));
		return nullptr;
	}
	auto cur_type = clang_getCursorType(_in_cursor);
	auto cursor_name = utils::full_name(_in_cursor);
	auto alias_type = clang_getTypedefDeclUnderlyingType(_in_cursor);
	auto alias_type_info = get_type(alias_type);
	the_logger.debug("get_alias_typedef cursor {} from type {} to type {}", cursor_name, utils::to_string(cur_type), utils::to_string(alias_type));
	auto result_type = new type_info(cursor_name, cur_type, alias_type_info);
	m_type_data[cursor_name] = result_type;
	return result_type;
}
bool type_db::add_class(class_node *_cur_class)
{
	if (!_cur_class)
	{
		return false;
	}
	auto cur_class_name = _cur_class->name();
	auto cur_iter = m_class_data.find(cur_class_name);
	if (cur_iter == m_class_data.end())
	{
		m_class_data[cur_class_name] = _cur_class;
		return true;
	}
	else
	{
		return false;
	}
}
bool type_db::add_enum(enum_node *_cur_enum)
{
	if (!_cur_enum)
	{
		return false;
	}
	auto cur_enum_name = _cur_enum->name();
	auto cur_iter = m_enum_data.find(cur_enum_name);
	if (cur_iter == m_enum_data.end())
	{
		m_enum_data[cur_enum_name] = _cur_enum;
		return true;
	}
	else
	{
		return false;
	}
}
class_node *type_db::get_class(const std::string &_class_name)
{
	auto cur_iter = m_class_data.find(_class_name);
	if (cur_iter == m_class_data.end())
	{
		return nullptr;
	}
	else
	{
		return cur_iter->second;
	}
}
json type_db::to_json() const
{
	json result;
	json type_data;
	for (const auto &one_item : m_type_data)
	{
		type_data[one_item.first] = one_item.second->to_json();
	}
	result["types"] = type_data;
	json class_data;
	for (const auto &one_item : m_class_data)
	{
		class_data[one_item.first] = one_item.second->to_json();
	}
	result["classes"] = class_data;
	result["template_types"] = utils::template_types::instance().all_values();

	return result;
}
void type_db::build_class_under_namespace(const std::string& ns_name)
{
	std::queue<language::node*> tasks;
	auto& all_ns_nodes = language::name_space::get_synonymous_name_spaces(ns_name);
	auto & the_logger = utils::get_logger();
	auto cur_visitor = [&ns_name, &the_logger](const language::node* m_node)
	{
		switch (m_node->get_kind())
		{
		case CXCursor_ClassTemplate:
		case CXCursor_ClassDecl:
		case CXCursor_StructDecl:
		{
			if (clang_isCursorDefinition(m_node->get_cursor()))
			{
				auto temp_node = new language::class_node(m_node);
				the_logger.info("new class {}", temp_node->to_json().dump(4));
				break;
			}
			else
			{
				the_logger.info("pre decl for class {}", m_node->get_name());
				break;
			}

		}
		case CXCursor_EnumDecl:
		{
			auto temp_node = new language::enum_node(m_node);
			the_logger.info("new enum {}", temp_node->to_json().dump(4));
			break;
		}
		case CXCursor_TypedefDecl:
		case CXCursor_TypeAliasDecl:
		{
			language::type_db::instance().get_alias_typedef(m_node->get_cursor());
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
void type_db::create_from_translate_unit(CXCursor _tu_cursor)
{
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
	clang_visitChildren(_tu_cursor, visitor, nullptr);
}
type_db::type_db()
{
}
void type_db::add_alternate_name(CXType _in_type, const std::string& annotated_typename)
{
	auto cur_type_info = get_type(_in_type);
	cur_type_info->m_pretty_name = annotated_typename;
	for (int i = 0; i < m_annotated_typenames.size(); i++)
	{
		if (cur_type_info->name().size() > m_annotated_typenames[i].first.size())
		{
			m_annotated_typenames.insert(m_annotated_typenames.begin() + i, std::make_pair(cur_type_info->name(), annotated_typename));
			return;
		}
	}
	m_annotated_typenames.emplace_back(cur_type_info->name(), annotated_typename);
	return;
}
} // namespace spiritsaway::meta::language
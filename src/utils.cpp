
#include "utils.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace
{
	std::shared_ptr<spdlog::logger> create_logger()
	{
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::warn);
		console_sink->set_pattern("[meta] [%^%l%$] %v");

		auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("meta.log", true);
		file_sink->set_level(spdlog::level::trace);
		auto logger = std::make_shared<spdlog::logger>("meta", spdlog::sinks_init_list{ console_sink, file_sink });
		logger->set_level(spdlog::level::trace);
		return logger;
	}
}

namespace meta::utils
{
	std::string to_string(const CXCursor& _in_cursor)
	{
		return to_string(clang_getCursorSpelling(_in_cursor));
	}
	std::string to_string(const CXString &str)
	{
		if (!str.data)
		{
			return "";
		}
		auto temp = clang_getCString(str);
		std::string result(temp);
		clang_disposeString(str);
		return result;
	}
	std::string to_string(const CXType& _in_type)
	{
		return to_string(clang_getTypeSpelling(_in_type));
	}
	std::string to_string(CXTypeKind _in_kind)
	{
		switch (_in_kind)
		{
		case CXType_Invalid:
			return "CXType_Invalid";
			break;
		case CXType_Unexposed:
			return "CXType_Unexposed";
			break;
		case CXType_Void:
			return "CXType_Void";
			break;
		case CXType_Bool:
			return "CXType_Bool";
			break;
		case CXType_Char_U:
			return "CXType_Char_U";
			break;
		case CXType_UChar:
			return "CXType_UChar";
			break;
		case CXType_Char16:
			return "CXType_Char16";
			break;
		case CXType_Char32:
			return "CXType_Char32";
			break;
		case CXType_UShort:
			return "CXType_UShort";
			break;
		case CXType_UInt:
			return "CXType_UInt";
			break;
		case CXType_ULong:
			return "CXType_ULong";
			break;
		case CXType_ULongLong:
			return "CXType_ULongLong";
			break;
		case CXType_UInt128:
			return "CXType_UInt128";
			break;
		case CXType_Char_S:
			return "CXType_Char_S";
			break;
		case CXType_SChar:
			return "CXType_SChar";
			break;
		case CXType_WChar:
			return "CXType_WChar";
			break;
		case CXType_Short:
			return "CXType_Short";
			break;
		case CXType_Int:
			return "CXType_Int";
			break;
		case CXType_Long:
			return "CXType_Long";
			break;
		case CXType_LongLong:
			return "CXType_LongLong";
			break;
		case CXType_Int128:
			return "CXType_Int128";
			break;
		case CXType_Float:
			return "CXType_Float";
			break;
		case CXType_Double:
			return "CXType_Double";
			break;
		case CXType_LongDouble:
			return "CXType_LongDouble";
			break;
		case CXType_NullPtr:
			return "CXType_NullPtr";
			break;
		case CXType_Overload:
			return "CXType_Overload";
			break;
		case CXType_Dependent:
			return "CXType_Dependent";
			break;
		case CXType_ObjCId:
			return "CXType_ObjCId";
			break;
		case CXType_ObjCClass:
			return "CXType_ObjCClass";
			break;
		case CXType_ObjCSel:
			return "CXType_ObjCSel";
			break;
		case CXType_Float128:
			return "CXType_Float128";
			break;
		case CXType_Half:
			return "CXType_Half";
			break;
		case CXType_Float16:
			return "CXType_Float16";
			break;
		case CXType_ShortAccum:
			return "CXType_ShortAccum";
			break;
		case CXType_Accum:
			return "CXType_Accum";
			break;
		case CXType_LongAccum:
			return "CXType_LongAccum";
			break;
		case CXType_UShortAccum:
			return "CXType_UShortAccum";
			break;
		case CXType_UAccum:
			return "CXType_UAccum";
			break;
		case CXType_ULongAccum:
			return "CXType_ULongAccum";
			break;

		case CXType_Complex:
			return "CXType_Complex";
			break;
		case CXType_Pointer:
			return "CXType_Pointer";
			break;
		case CXType_BlockPointer:
			return "CXType_BlockPointer";
			break;
		case CXType_LValueReference:
			return "CXType_LValueReference";
			break;
		case CXType_RValueReference:
			return "CXType_RValueReference";
			break;
		case CXType_Record:
			return "CXType_Record";
			break;
		case CXType_Enum:
			return "CXType_Enum";
			break;
		case CXType_Typedef:
			return "CXType_Typedef";
			break;
		case CXType_ObjCInterface:
			return "CXType_ObjCInterface";
			break;
		case CXType_ObjCObjectPointer:
			return "CXType_ObjCObjectPointer";
			break;
		case CXType_FunctionNoProto:
			return "CXType_FunctionNoProto";
			break;
		case CXType_FunctionProto:
			return "CXType_FunctionProto";
			break;
		case CXType_ConstantArray:
			return "CXType_ConstantArray";
			break;
		case CXType_Vector:
			return "CXType_Vector";
			break;
		case CXType_IncompleteArray:
			return "CXType_IncompleteArray";
			break;
		case CXType_VariableArray:
			return "CXType_VariableArray";
			break;
		case CXType_DependentSizedArray:
			return "CXType_DependentSizedArray";
			break;
		case CXType_MemberPointer:
			return "CXType_MemberPointer";
			break;
		case CXType_Auto:
			return "CXType_Auto";
			break;
		case CXType_Elaborated:
			return "CXType_Elaborated";
			break;
		case CXType_Pipe:
			return "CXType_Pipe";
			break;
		
		default:
			return std::to_string(_in_kind);
			break;
		}
		return "";
	}
	std::string join(const std::vector<std::string>& param, const std::string& sep)
	{
		std::string result = "";
		std::uint32_t total_size = 0;
		for (const auto& i : param)
		{
			total_size += i.size();
		}
		total_size += sep.size() * (std::max(param.size(), 1u) - 1);
		result.reserve(total_size);
		for (const auto& one_str : param)
		{
			if (!result.empty())
			{
				result += sep + one_str;
			}
			else
			{
				result = one_str;
			}
		}
		return result;
	}
	spdlog::logger & get_logger()
	{
		static std::shared_ptr<spdlog::logger> _logger = create_logger();
		return *_logger;
	}
	std::string to_string(CXTemplateArgumentKind _in_kind)
	{
		switch (_in_kind)
		{
		case CXTemplateArgumentKind_Null:
			return "CXTemplateArgumentKind_Null";
			break;
		case CXTemplateArgumentKind_Type:
			return "CXTemplateArgumentKind_Type";
			break;
		case CXTemplateArgumentKind_Declaration:
			return "CXTemplateArgumentKind_Declaration";
			break;
		case CXTemplateArgumentKind_NullPtr:
			return "CXTemplateArgumentKind_NullPtr";
			break;
		case CXTemplateArgumentKind_Integral:
			return "CXTemplateArgumentKind_Integral";
			break;
		case CXTemplateArgumentKind_Template:
			return "CXTemplateArgumentKind_Template";
			break;
		case CXTemplateArgumentKind_TemplateExpansion:
			return "CXTemplateArgumentKind_TemplateExpansion";
			break;
		case CXTemplateArgumentKind_Expression:
			return "CXTemplateArgumentKind_Expression";
			break;
		case CXTemplateArgumentKind_Pack:
			return "CXTemplateArgumentKind_Pack";
			break;
		case CXTemplateArgumentKind_Invalid:
			return "CXTemplateArgumentKind_Invalid";
			break;
		default:
			return "";
			break;
		}
	}
	const std::string& to_string(CXCursorKind _in_kind)
	{
		const static std::string unknown = "CXCursor_UnexposedDecl";
		static std::unordered_map<CXCursorKind, std::string> kind_str_map = {
		{CXCursor_UnexposedDecl, "CXCursor_UnexposedDecl"},
		{CXCursor_StructDecl,"CXCursor_StructDecl"},
		{CXCursor_UnionDecl,"CXCursor_UnionDecl"},
		{CXCursor_ClassDecl,"CXCursor_ClassDecl"},
		{CXCursor_EnumDecl,"CXCursor_EnumDecl"},
		{CXCursor_FieldDecl,"CXCursor_FieldDecl"},
		{CXCursor_EnumConstantDecl,"CXCursor_EnumConstantDecl"},
		{CXCursor_FunctionDecl,"CXCursor_FunctionDecl"},
		{CXCursor_VarDecl,"CXCursor_VarDecl"},
		{CXCursor_ParmDecl,"CXCursor_ParmDecl"},
		{CXCursor_TypedefDecl,"CXCursor_TypedefDecl"},
		{CXCursor_CXXMethod,"CXCursor_CXXMethod"},
		{CXCursor_Namespace,"CXCursor_Namespace"},
		{CXCursor_LinkageSpec,"CXCursor_LinkageSpec"},
		{CXCursor_Constructor,"CXCursor_Constructor"},
		{CXCursor_Destructor,"CXCursor_Destructor"},
		{CXCursor_ConversionFunction,"CXCursor_ConversionFunction"},
		{CXCursor_TemplateTypeParameter,"CXCursor_TemplateTypeParameter"},
		{CXCursor_NonTypeTemplateParameter,"CXCursor_NonTypeTemplateParameter"},
		{CXCursor_TemplateTemplateParameter,"CXCursor_TemplateTemplateParameter"},
		{CXCursor_FunctionTemplate,"CXCursor_FunctionTemplate"},
		{CXCursor_ClassTemplate,"CXCursor_ClassTemplate"},
		{CXCursor_ClassTemplatePartialSpecialization,"CXCursor_ClassTemplatePartialSpecialization"},
		{CXCursor_NamespaceAlias,"CXCursor_NamespaceAlias"},
		{CXCursor_UsingDirective,"CXCursor_UsingDirective"},
		{CXCursor_TypeAliasDecl,"CXCursor_TypeAliasDecl"},
		{CXCursor_CXXAccessSpecifier,"CXCursor_CXXAccessSpecifier"},
		{CXCursor_TypeRef,"CXCursor_TypeRef"},
		{CXCursor_TemplateRef,"CXCursor_TemplateRef"},
		{CXCursor_NamespaceRef,"CXCursor_NamespaceRef"},
		{CXCursor_MemberRef,"CXCursor_MemberRef"},
		{CXCursor_LabelRef,"CXCursor_LabelRef"},
		{CXCursor_OverloadedDeclRef,"CXCursor_OverloadedDeclRef"},
		{CXCursor_VariableRef,"CXCursor_VariableRef"},
		{CXCursor_UnexposedExpr,"CXCursor_UnexposedExpr"},
		{CXCursor_DeclRefExpr,"CXCursor_DeclRefExpr"},
		{CXCursor_MemberRefExpr,"CXCursor_MemberRefExpr"},
		{CXCursor_CallExpr,"CXCursor_CallExpr"},
		{CXCursor_BlockExpr,"CXCursor_BlockExpr"},
		{CXCursor_IntegerLiteral,"CXCursor_IntegerLiteral"},
		{CXCursor_FloatingLiteral,"CXCursor_FloatingLiteral"},
		{CXCursor_ImaginaryLiteral,"CXCursor_ImaginaryLiteral"},
		{CXCursor_StringLiteral,"CXCursor_StringLiteral"},
		{CXCursor_CharacterLiteral,"CXCursor_CharacterLiteral"},
		{CXCursor_UnaryOperator,"CXCursor_UnaryOperator"},
		{CXCursor_ArraySubscriptExpr,"CXCursor_ArraySubscriptExpr"},
		{CXCursor_BinaryOperator,"CXCursor_BinaryOperator"},
		{CXCursor_CompoundAssignOperator,"CXCursor_CompoundAssignOperator"},
		{CXCursor_ConditionalOperator,"CXCursor_ConditionalOperator"},
		{CXCursor_CStyleCastExpr,"CXCursor_CStyleCastExpr"},
		{CXCursor_CompoundLiteralExpr,"CXCursor_CompoundLiteralExpr"},
		{CXCursor_InitListExpr,"CXCursor_InitListExpr"},
		{CXCursor_CXXStaticCastExpr,"CXCursor_CXXStaticCastExpr"},
		{CXCursor_CXXDynamicCastExpr,"CXCursor_CXXDynamicCastExpr"},
		{CXCursor_CXXReinterpretCastExpr,"CXCursor_CXXReinterpretCastExpr"},
		{CXCursor_CXXConstCastExpr,"CXCursor_CXXConstCastExpr"},
		{CXCursor_CXXFunctionalCastExpr,"CXCursor_CXXFunctionalCastExpr"},
		{CXCursor_CXXBoolLiteralExpr,"CXCursor_CXXBoolLiteralExpr"},
		{CXCursor_CXXNullPtrLiteralExpr,"CXCursor_CXXNullPtrLiteralExpr"},
		{CXCursor_CXXThisExpr,"CXCursor_CXXThisExpr"},
		{CXCursor_CXXThrowExpr,"CXCursor_CXXThrowExpr"},
		{CXCursor_CXXNewExpr,"CXCursor_CXXNewExpr"},
		{CXCursor_CXXDeleteExpr,"CXCursor_CXXDeleteExpr"},
		{CXCursor_UnaryExpr,"CXCursor_UnaryExpr"},
		{CXCursor_PackExpansionExpr,"CXCursor_PackExpansionExpr"},
		{CXCursor_SizeOfPackExpr,"CXCursor_SizeOfPackExpr"},
		{CXCursor_LambdaExpr,"CXCursor_LambdaExpr"},
		{CXCursor_UnexposedStmt,"CXCursor_UnexposedStmt"},
		{CXCursor_LabelStmt,"CXCursor_LabelStmt"},
		{CXCursor_CompoundStmt,"CXCursor_CompoundStmt"},
		{CXCursor_CaseStmt,"CXCursor_CaseStmt"},
		{CXCursor_DefaultStmt,"CXCursor_DefaultStmt"},
		{CXCursor_IfStmt,"CXCursor_IfStmt"},
		{CXCursor_SwitchStmt,"CXCursor_SwitchStmt"},
		{CXCursor_WhileStmt,"CXCursor_WhileStmt"},
		{CXCursor_DoStmt,"CXCursor_DoStmt"},
		{CXCursor_ForStmt,"CXCursor_ForStmt"},
		{CXCursor_GotoStmt,"CXCursor_GotoStmt"},
		{CXCursor_IndirectGotoStmt,"CXCursor_IndirectGotoStmt"},
		{CXCursor_BreakStmt,"CXCursor_BreakStmt"},
		{CXCursor_ReturnStmt,"CXCursor_ReturnStmt"},
		{CXCursor_AsmStmt,"CXCursor_AsmStmt"},
		{CXCursor_CXXCatchStmt,"CXCursor_CXXCatchStmt"},
		{CXCursor_CXXTryStmt,"CXCursor_CXXTryStmt"},
		{CXCursor_CXXForRangeStmt,"CXCursor_CXXForRangeStmt"},
		{CXCursor_UnexposedAttr,"CXCursor_UnexposedAttr"},
		{CXCursor_CXXFinalAttr,"CXCursor_CXXFinalAttr"},
		{CXCursor_CXXOverrideAttr,"CXCursor_CXXOverrideAttr"},
		{CXCursor_AnnotateAttr,"CXCursor_AnnotateAttr"},
		{CXCursor_FriendDecl,"CXCursor_FriendDecl"},
		};
		auto cur_iter = kind_str_map.find(_in_kind);
		if (cur_iter == kind_str_map.end())
		{
			return unknown;
		}
		else
		{
			return cur_iter->second;
		}
	}
	const std::string& get_qualified_name_from_cursor(CXCursor in_cursor)
	{
		static std::unordered_map< CXCursor, std::string, cursor_hash, cursor_equal> qualified_cache;
		auto cur_iter = qualified_cache.find(in_cursor);
		if (cur_iter != qualified_cache.end())
		{
			return cur_iter->second;
		}
		std::string result;
		if (in_cursor.kind == CXCursorKind::CXCursor_TranslationUnit)
		{
			qualified_cache[in_cursor] = meta::utils::to_string(in_cursor);
		}
		else
		{
			auto parent = clang_getCursorSemanticParent(in_cursor);
			
			if (clang_isInvalid(parent.kind))
			{
				qualified_cache[in_cursor] = meta::utils::to_string(in_cursor);
				
			}
			else
			{
				const std::string& parent_name = get_qualified_name_from_cursor(parent);
				if (parent_name.empty())
				{
					qualified_cache[in_cursor] = meta::utils::to_string(in_cursor);
				}
				else
				{
					if (parent.kind == CXCursorKind::CXCursor_TranslationUnit)
					{
						qualified_cache[in_cursor] = meta::utils::to_string(in_cursor);
					}
					else
					{
						qualified_cache[in_cursor] = get_qualified_name_from_cursor(parent) + "::" + meta::utils::to_string(in_cursor);
					}

				}
			}
			

		}
		return qualified_cache[in_cursor];

	}


}

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

	const std::string& cursor_kind_to_string(CXCursorKind _in_kind)
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
}
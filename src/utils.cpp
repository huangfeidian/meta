
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

	std::vector<std::string_view> get_tokens(const std::string& cur_annotation)
	{
		if (cur_annotation.empty())
		{
			return {};
		}
		std::vector<std::string_view> token_result = {};
		std::size_t begin_index = 0;
		for (std::size_t i = 0; i < cur_annotation.size(); i++)
		{
			auto cur_char = cur_annotation[i];
			switch (cur_char)
			{
			case ' ':
			case '\t':
				if (begin_index != i)
				{
					token_result.push_back(std::string_view(&cur_annotation[begin_index], i - begin_index));
				}
				begin_index = i + 1;
				break;
			case '(':
			case ')':
			case ',':
				if (begin_index != i)
				{
					token_result.push_back(std::string_view(&cur_annotation[begin_index], i - begin_index));
				}
				token_result.push_back(std::string_view(&cur_annotation[i], 1));
				begin_index = i + 1;
				break;
			default:
				break;
			}
		}
		if (begin_index != cur_annotation.size())
		{
			token_result.push_back(std::string_view(&cur_annotation[begin_index], cur_annotation.size() - begin_index));
		}
		return token_result;
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
		
		const auto& template_type_name = utils::template_types::instance().get_type(_in_type);
		if (!template_type_name.empty())
		{
			return template_type_name;
		}
		auto argu_num = clang_Type_getNumTemplateArguments(_in_type);
		if (argu_num > 0)
		{
			// 这里需要对模板参数进行展开
			auto template_full_name = full_name(clang_getTypeDeclaration(_in_type));
			auto canonical_type = clang_getCanonicalType(_in_type);
			if (!clang_equalTypes(canonical_type, _in_type))
			{
				template_full_name = full_name(canonical_type);
			}
			auto template_anchor = template_full_name.find("<");
			if (template_anchor != std::string::npos)
			{
				template_full_name = template_full_name.substr(0, template_anchor);
			}
			
			std::vector<std::string> arg_names;
			for (int i = 0; i < argu_num; i++)
			{
				auto temp_arg_type = clang_Type_getTemplateArgumentAsType(_in_type, i);
				arg_names.push_back(to_string(temp_arg_type));

			}
			return template_full_name + "<" + join(arg_names, ",") + ">";
		}
		auto canonical_type = clang_getCanonicalType(_in_type);
		if (!clang_equalTypes(canonical_type, _in_type))
		{
			return full_name(canonical_type);
		}
		else
		{
			return to_string(clang_getTypeSpelling(_in_type));
		}
		
	}
	std::string full_name(const CXType& _in_type)
	{
		auto decl_cursor = clang_getTypeDeclaration(_in_type);

		auto& result = full_name(decl_cursor);
		if (result.empty())
		{
			return utils::to_string(_in_type);
		}
		return result;
	}
	std::string to_string(CXTypeKind _in_kind)
	{
		return to_string(clang_getTypeKindSpelling(_in_kind));
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
	const std::string& full_name(CXCursor in_cursor)
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
				const std::string& parent_name = full_name(parent);
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
						qualified_cache[in_cursor] = full_name(parent) + "::" + meta::utils::to_string(in_cursor);
					}

				}
			}
			

		}
		return qualified_cache[in_cursor];

	}

	std::vector<CXCursor> cursor_get_children(CXCursor parent)
	{
		std::vector<CXCursor> children;

		auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data)
		{
			auto container = static_cast<std::vector<CXCursor> *>(data);

			container->emplace_back(cursor);

			if (cursor.kind == CXCursor_LastPreprocessing)
				return CXChildVisit_Break;

			return CXChildVisit_Continue;
		};

		clang_visitChildren(parent, visitor, &children);
		return children;
	}
	CXTypeKind expect_std_int(CXType int_type)
	{
		const static std::unordered_map<std::string, CXTypeKind> std_int_map = {
			{"std::int8_t", CXType_SChar},
			{"std::uint8_t", CXType_UChar},
			{"std::int16_t", CXType_Short},
			{"std::uint16_t", CXType_UShort},
			{"std::int32_t", CXType_Int},
			{"std::uint32_t", CXType_UInt},
			{"std::int64_t", CXType_LongLong},
			{"std::uint64_t", CXType_ULongLong}
		};
		if (int_type.kind != CXType_Elaborated)
		{
			return int_type.kind;
		}
		else
		{
			auto cur_iter = std_int_map.find(to_string(int_type));
			if (cur_iter != std_int_map.end())
			{
				return cur_iter->second;
			}
			else
			{
				return CXType_Invalid;
			}

		}

	}
	std::size_t cursor_hash::operator()(const CXCursor& in_cursor) const
	{
		return clang_hashCursor(in_cursor);
	}

	bool cursor_equal::operator()(const CXCursor& from, const CXCursor& to) const
	{
		return clang_equalCursors(from, to);
	}
	std::size_t cxtype_hash::operator()(const CXType& in_type) const
	{
		return reinterpret_cast<std::size_t>(in_type.data[0]) + reinterpret_cast<std::size_t>(in_type.data[1]);
	}
	bool cxtype_equal::operator()(const CXType& type_1, const CXType& type_2) const
	{
		return clang_equalTypes(type_1, type_2);
	}
	template_types::template_types()
	{

	}
	template_types& template_types::instance()
	{
		static template_types _instance;
		return _instance;
	}
	bool template_types::add_type(const CXCursor& template_name_cursor)
	{
		if (clang_getCursorKind(template_name_cursor) != CXCursor_TemplateTypeParameter)
		{
			return false;
		}
		auto& cursor_name = full_name(template_name_cursor);
		auto cursor_type = clang_getCursorType(template_name_cursor);
		if (cursor_name.empty())
		{
			return false;
		}
		auto cur_iter = _type_cache.find(cursor_type);
		if (cur_iter == _type_cache.end())
		{
			_type_cache[cursor_type] = cursor_name;
			return true;
		}
		else
		{
			return false;
		}

	}
	const std::string& template_types::get_type(const CXType& cur_type)const
	{
		static std::string invalid_result = "";
		auto cur_iter = _type_cache.find(cur_type);
		if (cur_iter == _type_cache.end())
		{
			return invalid_result;
		}
		else
		{
			return cur_iter->second;
		}

	}
	std::vector<std::string> template_types::all_values() const
	{
		std::vector<std::string> result;
		for (const auto& i : _type_cache)
		{
			result.emplace_back(i.second);
		}
		return result;
	}
	std::unordered_map<std::string, std::vector<std::string>> parse_annotation(const std::string& cur_annotation)
	{
		std::vector<std::string_view>& _tokens = get_tokens(cur_annotation);
		auto& the_logger = meta::utils::get_logger();
		the_logger.debug("parse_annotation for {} with token_size {}", cur_annotation, _tokens.size());
		std::unordered_map<std::string, std::vector<std::string>> result;
		bool wait_for_right_paren = false;
		std::string cur_key;
		std::vector<std::string> cur_values;
		for (auto token : _tokens)
		{
			switch (token.size())
			{
			case 1:
				switch (token[0])
				{
				case '(':
					if (wait_for_right_paren || cur_key.empty() || !cur_values.empty())
					{
						the_logger.error("fail to parse annotation from {}", cur_annotation);
						return {};
					}
					wait_for_right_paren = true;
					break;
				case ')':
					if (wait_for_right_paren)
					{
						wait_for_right_paren = false;
						result[cur_key] = cur_values;
						cur_values.clear();
						cur_key.clear();
					}
					else
					{
						the_logger.error("fail to parse annotation from {}", cur_annotation);
						return {};
					}
					break;
				case ',':
					if (!wait_for_right_paren)
					{
						if (!cur_key.empty())
						{
							result[cur_key] = {};
							cur_key.clear();
						}
						
					}
					break;
				default:
					if (wait_for_right_paren)
					{
						cur_values.push_back(std::string(token));
					}
					else
					{
						cur_key = std::string(token);
					}
					break;
				}
				break;
			default:
				if (wait_for_right_paren)
				{
					cur_values.push_back(std::string(token));
				}
				else
				{
					cur_key = std::string(token);
				}
				break;
			}

		}
		if (wait_for_right_paren)
		{
			the_logger.error("fail to parse annotation from {}", cur_annotation);
			return {};
		}
		if (!cur_key.empty())
		{
			result[cur_key] = {};
		}
		return result;
	}
}
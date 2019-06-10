#include "nodes/type_info.h"
namespace
{
    
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
    
}
namespace meta::language
{
	type_info::type_info(const std::string& _in_name, enum CXTypeKind _in_kind):
		_kind(_in_kind),
		_ref_type(nullptr),
		_name(_in_name)
	{

	}
	type_info::type_info(const std::string& _in_name, const CXType& _in_type, const type_info* _in_ref_type):
		_type(_in_type),
		_kind(_in_type.kind),
		_ref_type(_in_ref_type),
		_name(_in_name)
	{

	}
	const std::string& type_info::name() const
	{
		return _name;
	}
	std::string type_info::qualified_name() const
	{
		auto the_cursor = clang_getTypeDeclaration(_type);
		return utils::get_qualified_name_from_cursor(the_cursor);
	}
	const CXType& type_info::type() const
	{
		return _type;
	}
	enum CXTypeKind type_info::kind() const
	{
		return _kind;
	}
	const type_info* type_info::ref_type() const
	{
		return _ref_type;
	}
	void type_info::set_type(CXType _in_type)
	{
		_type = _in_type;
		_kind = _type.kind;
	}
	const std::vector<const type_info*>& type_info::template_args() const
	{
		return _template_args;
	}
	bool type_info::is_completed() const
	{
		if (_kind == CXTypeKind::CXType_Unexposed)
		{
			auto decl_type = clang_getTypeDeclaration(_type);
			if (decl_type.kind == CXTypeKind::CXType_Invalid)
			{
				return false;
			}
			return true;
		}
		if (_template_args.empty())
		{
			return true;
		}
		for (auto i : _template_args)
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
		return !_template_args.empty();
	}
	bool type_info::is_callable() const
	{
		return false;
	}
	bool type_info::is_const() const
	{
		return clang_isConstQualifiedType(_type);
	}
	bool type_info::is_reference() const
	{
		return _kind == CXTypeKind::CXType_LValueReference || _kind == CXTypeKind::CXType_RValueReference;
	}
	bool type_info::is_lvalue_refer() const
	{
		return _kind == CXTypeKind::CXType_LValueReference;
	}
	bool type_info::is_rvalue_refer() const
	{
		return _kind == CXTypeKind::CXType_RValueReference;
	}
	bool type_info::is_pointer() const
	{
		return _kind == CXTypeKind::CXType_Pointer || _kind == CXTypeKind::CXType_BlockPointer;
	}
	const type_info* type_info::point_to() const
	{
		if (!is_pointer())
		{
			return nullptr;
		}
		return _ref_type;
	}
	const type_info* type_info::refer_to() const
	{
		if (!is_reference())
		{
			return nullptr;
		}
		return _ref_type;
	}
	json type_info::to_json() const
	{
		json result;
		result["name"] = _name;
		result["qualified_name"] = qualified_name();
		result["kind"] = static_cast<std::uint32_t>(_kind);
		if (_ref_type)
		{
			result["ref_type"] = _ref_type->qualified_name();
		}
		if (!_template_args.empty())
		{
			std::vector<std::string> args;
			for (const auto i : _template_args)
			{
				args.push_back(i->qualified_name());
			}
			result["args"] = args;
		}
		return result;
	}

	type_info* type_db::get_type_for_const(const CXType& _in_type)
	{
		auto full_name = utils::to_string(_in_type);
		auto cur_kind = _in_type.kind;
		auto decl_cursor = clang_getTypeDeclaration(_in_type);
		if (clang_Cursor_isNull(decl_cursor))
		{
			if (cur_kind >= CXType_Void && cur_kind <= CXType_Float16)
			{
				// for const T with T be builtin types
				// for example const int
				auto non_const_name = full_name.substr(6);// "const int" -> "int"
				type_info* non_const_type;
				auto non_const_type_iter = _type_data.find(non_const_name);

				if (non_const_type_iter == _type_data.end())
				{
					non_const_type = new type_info(non_const_name, cur_kind);
					_type_data[non_const_name] = non_const_type;
				}
				else
				{
					non_const_type = non_const_type_iter->second;
				}


				auto final_result = new type_info(full_name, _in_type, non_const_type);
				_type_data[full_name] = final_result;
				return final_result;
			}
			else
			{
				
				auto pointer_to_type = clang_getPointeeType(_in_type);
				if (pointer_to_type.kind != CXTypeKind::CXType_Invalid)
				{
					// for const T *const
					auto pointee_type = get_type(pointer_to_type); // for const T
					const auto& temp_name = pointee_type->name();
					std::string pointer_to_type_name = temp_name + " *";
					type_info* temp_result = nullptr;
					auto temp_iter = _type_data.find(pointer_to_type_name);
					if (temp_iter == _type_data.end())
					{
						temp_result = new type_info(pointer_to_type_name, _in_type, pointee_type);
					}
					else
					{
						temp_result = temp_iter->second;
					}
					auto final_result = new type_info(full_name, _in_type, temp_result);
					
					_type_data[full_name] = final_result;
					return final_result;

				}
				else
				{
					// some error case I cant handle
					auto final_result = new type_info(full_name, _in_type, nullptr);
					_type_data[full_name] = final_result;
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
			_type_data[full_name] = final_type;
			return final_type;
		}
	}
	type_info* type_db::get_type_for_pointee(const CXType& _in_type)
	{
		auto full_name = utils::to_string(_in_type);
		auto pointer_to_type = clang_getPointeeType(_in_type);
		auto pointee_type = get_type(pointer_to_type);
		auto final_result = new type_info(full_name, _in_type, pointee_type);
		_type_data[full_name] = final_result;
		return final_result;
	}
	type_info* type_db::get_type_for_template(const CXType& _in_type)
	{
		auto full_name = utils::to_string(_in_type);
		auto argu_num = clang_Type_getNumTemplateArguments(_in_type);
		auto base_type = get_type(clang_getTypeDeclaration(_in_type));
		std::vector<type_info*> arg_types;
		for (int i = 0; i < argu_num; i++)
		{
			auto temp_arg_type = get_type(clang_Type_getTemplateArgumentAsType(_in_type, i));
			arg_types.push_back(temp_arg_type);

		}
		auto final_result = new type_info(full_name, _in_type, base_type);
		_type_data[full_name] = final_result;
		return final_result;
	}

	type_info* type_db::get_type(const CXType& _in_type)
	{
		auto full_name = utils::to_string(_in_type);
		auto type_iter = _type_data.find(full_name);
		if (type_iter != _type_data.end())
		{
			auto result = type_iter->second;
			if (result->_type.kind != CXTypeKind::CXType_Invalid)
			{
				result->set_type(_in_type);
			}
			return type_iter->second;
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
		_type_data[full_name] = final_type;
		return final_type;
	}
}
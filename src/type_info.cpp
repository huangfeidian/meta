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
		ref_type(nullptr),
		_name(_in_name)
	{

	}
	type_info::type_info(const std::string& _in_name, const CXType& _in_type, const type_info* _in_ref_type):
		_type(_in_type),
		_kind(_in_type.kind),
		ref_type(_in_ref_type),
		_name(_in_name)
	{

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
				// some error case I cant handle
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
						auto temp_result = new type_info(full_name, _in_type, pointee_type);
					}

					
					_type_data[full_name] = final_result;
					return final_result;

				}
				auto final_result = new type_info(full_name, _in_type, nullptr);
				_type_data[full_name] = final_result;
				return final_result;
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
	type_info* type_db::get_type(const CXType& _in_type)
	{
		auto full_name = utils::to_string(_in_type);
		auto type_iter = _type_data.find(full_name);
		if (type_iter != _type_data.end())
		{
			return type_iter->second;
		}
		auto is_const = clang_isConstQualifiedType(_in_type);
		auto cur_kind = _in_type.kind;
		type_info* result;
		if (is_const)
		{
			return get_type_for_const(_in_type);
		}
		auto pointer_to_type = clang_getPointeeType(_in_type);
		
		if (pointer_to_type.kind != CXTypeKind::CXType_Invalid)
		{
			// is a reference type but not a const T* const
			// is not a reference type

		}
		else
		{
			result = get_type(pointer_to_type);
		}

		return result;
	}
}
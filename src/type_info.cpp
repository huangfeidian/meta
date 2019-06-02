#include "type_info.h"
namespace
{
    bool is_const_type(CXType _cur_type)
    {
        auto is_const = clang_isConstQualifiedType(_cur_type);
        if(is_const)
        {
            return true;
        }
        if(!is_reference_type(_cur_type))
        {
            return false;
        }
        auto ref_type = clang_getPointeeType(_cur_type);
        if(ref_type.kind)
        {
            return clang_isConstQualifiedType(ref_type);
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
        return _cur_type.kind == CXType_LValueReference
    }
    bool is_rvalue_refer(CXType _cur_type)
    {
        return _cur_type.kind == CXType_RValueReference;
    }
    
}
namespace meta::language
{
    
}
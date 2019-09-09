#include <meta/parser/nodes/enum.h>

namespace meta::language
{
	enum_node::enum_node(const node* _in_node):
		node_base(_in_node)
	{
		parse();

	}
	void enum_node::parse()
	{
		auto& cur_name = name();
		auto& the_logger = utils::get_logger();
		std::vector<CXCursor> children = utils::cursor_get_children(get_node()->get_cursor());
		for (const auto& i : children)
		{
			switch (clang_getCursorKind(i))
			{
			case CXCursor_EnumConstantDecl:
			{
				auto cur_key_name = utils::to_string(i);
				auto cur_value = clang_getEnumConstantDeclUnsignedValue(i);
				_keys.push_back(std::move(cur_key_name));
				_values.push_back(cur_value);
				break;
			}
			default:
				break;
			}
		}
		if (!type_db::instance().add_enum(this))
		{
			the_logger.warn("fail to add enum {} to type_db", name());
		}
		auto _enum_value_type = clang_getEnumDeclIntegerType(get_node()->get_cursor());
		auto _enum_value_name = utils::to_string(_enum_value_type);
		
		auto cur_int_kind = utils::expect_std_int(_enum_value_type);
		the_logger.info("enum {} has value_type {} kind_name {}", name(), _enum_value_name, utils::to_string(cur_int_kind));
		switch (cur_int_kind)
		{
		case CXType_SChar:
			_int_width = 8;
			break;
		case CXType_UChar:
			_int_width = 8;
			break;
		case CXType_Short:
			_int_width = 16;
			break;
		case CXType_UShort:
			_int_width = 16;
			break;
		case CXType_Int:
			_int_width = 32;
			break;
		case CXType_UInt:
			_int_width = 32;
			break;
		case CXType_Long:
			_int_width = 32;
			break;
		case CXType_ULong:
			_int_width = 32;
			break;
		case CXType_LongLong:
			_int_width = 64;
			break;
		case CXType_ULongLong:
			_int_width = 64;
			break;
		case CXType_Elaborated:
			
		default:
			_int_width = 32;

		}
	}
	const std::vector<std::string>& enum_node::keys() const
	{
		return _keys;
	}
	const std::vector<std::uint64_t>& enum_node::values() const
	{
		return _values;
	}
	std::uint64_t enum_node::value_for(const std::string& _in_key) const
	{
		for (std::size_t i = 0; i < _keys.size(); i++)
		{
			if (_keys[i] == _in_key)
			{
				return _values[i];
			}
		}
		return 0;
	}
	std::uint8_t enum_node::value_int_width() const
	{
		return _int_width;
	}
	json enum_node::to_json()const
	{
		json result;
		result["name"] = name();
		result["qualified_name"] = qualified_name();
		result["node_type"] = "enum";
		json values;
		for (std::size_t i = 0; i < _keys.size(); i++)
		{
			values[_keys[i]] = _values[i];
		}
		result["values"] = values;
		result["value_int_width"] = _int_width;
		return result;
	}

}
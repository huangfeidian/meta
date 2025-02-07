#include <meta/parser/nodes/enum.h>

namespace spiritsaway::meta::language
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
				m_keys.push_back(std::move(cur_key_name));
				m_values.push_back(cur_value);
				break;
			}
			default:
				break;
			}
		}
		if (!type_db::instance().add_enum(this))
		{
			the_logger.debug("fail to add enum {} to type_db", name());
		}
		auto _enum_value_type = clang_getEnumDeclIntegerType(get_node()->get_cursor());
		auto _enum_value_name = utils::to_string(_enum_value_type);
		
		auto cur_int_kind = utils::expect_std_int(_enum_value_type);
		the_logger.info("enum {} has value_type {} kind_name {}", name(), _enum_value_name, utils::to_string(cur_int_kind));
		switch (cur_int_kind)
		{
		case CXType_SChar:
			m_int_width = 8;
			break;
		case CXType_UChar:
			m_int_width = 8;
			break;
		case CXType_Short:
			m_int_width = 16;
			break;
		case CXType_UShort:
			m_int_width = 16;
			break;
		case CXType_Int:
			m_int_width = 32;
			break;
		case CXType_UInt:
			m_int_width = 32;
			break;
		case CXType_Long:
			m_int_width = 32;
			break;
		case CXType_ULong:
			m_int_width = 32;
			break;
		case CXType_LongLong:
			m_int_width = 64;
			break;
		case CXType_ULongLong:
			m_int_width = 64;
			break;
		case CXType_Elaborated:
			
		default:
			m_int_width = 32;

		}
	}
	const std::vector<std::string>& enum_node::keys() const
	{
		return m_keys;
	}
	const std::vector<std::uint64_t>& enum_node::values() const
	{
		return m_values;
	}
	std::uint64_t enum_node::value_for(const std::string& _in_key) const
	{
		for (std::size_t i = 0; i < m_keys.size(); i++)
		{
			if (m_keys[i] == _in_key)
			{
				return m_values[i];
			}
		}
		return 0;
	}
	std::uint8_t enum_node::value_int_width() const
	{
		return m_int_width;
	}
	json enum_node::to_json()const
	{
		json result;
		result["name"] = name();
		result["qualified_name"] = qualified_name();
		result["node_type"] = "enum";
		if (!comment().empty())
		{
			result["comment"] = comment();
		}
		json values;
		for (std::size_t i = 0; i < m_keys.size(); i++)
		{
			values[m_keys[i]] = m_values[i];
		}
		result["values"] = values;
		result["value_int_width"] = m_int_width;
		return result;
	}

}
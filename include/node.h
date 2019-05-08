#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <clang-c/Index.h>


namespace meta
{
	class language_node
	{
	public:
		std::uint32_t _id;
		std::string name;	
		language_node* _parent;
		CXTypeKind _kind;
		CXCursor _cursor; 
		std::vector<language_node*> _children;
		const static std::uint32_t invalid = 0;
	};

	class node_db
	{
	protected:
		std::unordered_map<std::uint32_t, language_node*> _nodes;

	};
}
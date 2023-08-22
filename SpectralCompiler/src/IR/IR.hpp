#pragma once
#include <cstdint>
#include <unordered_map>


// Holds special IR for my language to assist with codegen
namespace IR
{
	enum struct ctype
	{
		NIL, BOOLEAN, NUMBER, STRING, TABLE
	};

	// base type
	struct node_t
	{

	};

	struct table_t : node_t
	{

	};

	struct string_t : node_t
	{
		const char* value;
		std::size_t len;
	};

	struct global_t : node_t
	{

	};

	struct local_t : node_t
	{

	};

	struct upvalue_t : node_t
	{
		
	};

	struct constant_t : node_t
	{
		union
		{
			std::uint8_t u8;
			double n64;
			std::uint64_t u64;
			std::int64_t s64;
			string_t str;
			table_t table;
		};
		ctype type;
	};

	struct function_t : node_t
	{
	public:
		bool is_runtime = false;
		std::vector<local_t> locals{};
		std::vector<upvalue_t> upvalues{};

	};

	// IR context holds all necessary information grouped together
	class context_t : node_t
	{
	public:
		std::unordered_map<global_t, node_t> globals{};
		std::vector<function_t> functions{};
		std::size_t entry_function = 0;

	};
}
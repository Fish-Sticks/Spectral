#pragma once
#include <cstdint>
#include <memory>
#include <string>

// Special structure for holding memory, labeling memory regions and detecting out of bounds access.
#pragma pack(push, 1)
struct memory_manager_t
{
private:
	static constexpr std::size_t const& memory_size = 0xFFFF;
public:
	memory_manager_t() = default;
	memory_manager_t(const memory_manager_t&) = delete;
	std::uint8_t& operator[](std::size_t index)
	{
		return this->read_8(index);
	}

	// Read 8 bits of data from the memory block.
	constexpr std::uint8_t& read_8(std::size_t index);

	// Read 16 bits of data from the memory block.
	constexpr std::uint16_t read_16(std::size_t index);

	// Read the address of the data at index and dereference it, returning 8 bits.
	std::uint8_t& read_address_8(std::size_t index);

	// Read the address of the data at index and dereference it returning 16 bits.
	std::uint16_t read_address_16(std::size_t index);

	// Dump all of the memory block to a hex file for analysis.
	void dump_state(const std::string& file_name);

	// The actual block of memory
	union
	{
		std::uint8_t memory_block[memory_size]{ 0x0 };
		struct
		{
			int some_data;
		};
	};
};
#pragma pack(pop)
static_assert(sizeof(memory_manager_t) == 0xFFFF, "Memory manager is incorrect size!");
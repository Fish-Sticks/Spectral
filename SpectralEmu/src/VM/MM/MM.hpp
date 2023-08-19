#pragma once
#include <cstdint>
#include <memory>
#include <string>

// Special structure for holding memory, labeling memory regions and detecting out of bounds access.
#pragma pack(push, 1)
struct memory_manager_t
{
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

	// Read the address of the data at index and dereference it returning 16 bits. Optionally add offset to acquired ptr.
	std::uint16_t read_address_16(std::size_t index, std::uint16_t offset = 0);

	// Dump all of the memory block to a hex file for analysis.
	void dump_state(const std::string& file_name);

	// Load rom into memory, automatically calculate entrypoint, this takes raw data NOT a file.
	void load_rom(const std::string& rom_data, std::size_t base_address = 0);

	// In theory you can extend this to add hidden "supervisor code" but the instructions can never reach past 0xFFFF, 0xFFFF itself must be indexable too, thus +1
	static constexpr std::size_t const& memory_size = 0xFFFF + 1;

	// The actual block of memory
	union
	{
		std::uint8_t block[memory_size]{ 0x0 }; // Block of data which holds ALL memory entirely
		struct // Special defined memory locations
		{
			std::uint8_t zp[0xFF+1]; // Fast access memory, the zero page is a one byte page for fast r/w, 0 is an address along with 0xFF so +1
			std::uint8_t stack[0xFF]; // Stack which holds 256 bytes of memory, LIFO top-down stack model.
			std::uint8_t misc[0xFDFB]; // Misc data, user puts whatever they want in this area ( they are not limited to this area though ).
			std::uint16_t NMI; // Non-Maskable Interrupt vector
			std::uint16_t RES; // Reset vector ( can be used as start/reset, after 7 cycles the address stored here is read and at the 8th cycle it jumps to this entrypoint ). TLDR; an entrypoint
			std::uint16_t IRQ; // Interrupt Request vector.
		};
	};
};
#pragma pack(pop)
static_assert(sizeof(memory_manager_t) == memory_manager_t::memory_size, "Memory manager is incorrect size!");
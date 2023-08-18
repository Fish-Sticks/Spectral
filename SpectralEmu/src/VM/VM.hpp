#pragma once
#include "MM/MM.hpp"

#include <string>
#include <cstdint>

// Holds the context for the virtual machine, status flags, registers, etc
struct vm_context_t
{
private:
	// Set flags for comparisons
	void set_comparison_flags(std::uint8_t compared_against, std::uint8_t operand);
public:
	bool initialized = false;

	// Setup virtual machine to prepare for execution
	void setup_vm(const std::string& machine_code);

	// Clear VM and free resources.
	void clear_vm();

	// Dump VM context
	void dump_ctx();

	// Start VM and run it (if initialized)
	void run_vm();

	std::unique_ptr<memory_manager_t> memory_manager{};
	std::string machine_code{};
	std::string memory_block{};

	std::uint16_t PC = 0;	// Program counter
	std::uint8_t SP = 0;	// Stack pointer
	std::uint8_t AC = 0;	// Accumulator
	std::uint8_t X = 0;		// X register
	std::uint8_t Y = 0;		// Y register
	union
	{
		struct
		{
			std::uint8_t C : 1; // Carry
			std::uint8_t Z : 1; // Zero
			std::uint8_t I : 1; // Interrupt (IRQ disable)
			std::uint8_t D : 1; // Decimal (use BCD for arithmetics)
			std::uint8_t B : 1; // Break
			std::uint8_t _ : 1; // ignored ( possibly replace with initialized? )
			std::uint8_t V : 1; // Overflow
			std::uint8_t N : 1; // Negative
		} SR_flags;

		std::uint8_t SR = 0; // Status register
	};
};
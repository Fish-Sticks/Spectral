#pragma once
#include "VM/VM.hpp"

#include <cstdint>
#include <string>

class emulator_t
{
private:
	vm_context_t vm_context{};
public:
	emulator_t();
	~emulator_t();

	// Load emulator with raw executable code, base address is forced on here because this could just contain normal raw code instead of a complete binary.
	void load_emulator(const std::string& executable, std::size_t base_address);

	// Load emulator with a path to a file with executable code
	void load_emulator_path(const std::string& executable_path, std::size_t base_address = 0);

	// Run script
	void run_emulator();

	// Dump emulator memory with an optional custom name
	void dump_memory(const std::string& dump_name = "memdump.bin");
};
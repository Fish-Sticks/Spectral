#pragma once
#include <cstdint>
#include <string>
#include "VM/VM.hpp"

class emulator_t
{
private:
	vm_context_t vm_context{};
public:
	emulator_t();
	~emulator_t();

	// Load emulator with raw executable code
	void load_emulator(const std::string& executable);

	// Load emulator with a path to a file with executable code
	void load_emulator_path(const std::string& executable_path);

	// Run script
	void run_emulator();

	// Temporary test
	void testing();
};
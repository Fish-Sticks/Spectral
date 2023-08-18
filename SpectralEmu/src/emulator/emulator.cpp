#include "emulator.hpp"

emulator_t::emulator_t()
{
	std::printf("Emulator constructed!\n");
}

emulator_t::~emulator_t()
{
	std::printf("Emulator deconstructed!\n");
}

// Load emulator with raw executable code
void emulator_t::load_emulator(const std::string& executable)
{
	if (!this->vm_context.initialized)
	{
		this->vm_context.setup_vm(executable);
	}
	else
	{
		std::printf("VM is already initialized!\n");
	}
}

// Load emulator with a path to a file with executable code
void emulator_t::load_emulator_path(const std::string& executable_path)
{
	std::printf("load emulator path not a thing yet!\n");
}

// Run script
void emulator_t::run_emulator()
{
	this->vm_context.run_vm();
}


void emulator_t::testing()
{
	this->vm_context.memory_manager->dump_state("testing.bin");
}
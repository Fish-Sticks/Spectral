#include "emulator.hpp"

#include <fstream>

emulator_t::emulator_t()
{
	std::printf("Emulator constructed!\n");
}

emulator_t::~emulator_t()
{
	std::printf("Emulator deconstructed!\n");
}

void emulator_t::load_emulator(const std::string& executable, std::size_t base_address)
{
	if (!this->vm_context.initialized)
	{
		this->vm_context.setup_vm(executable, base_address);
	}
	else
		throw std::exception("VM is already initialized");
}

void emulator_t::load_emulator_path(const std::string& executable_path, std::size_t base_address)
{
	if (this->vm_context.initialized)
		throw std::exception("VM is already initialized");

	std::string program_data{};
	{
		std::fstream program_file{ executable_path, std::fstream::binary | std::fstream::in };
		while (!program_file.eof())
		{
			program_data.push_back(program_file.get());
		}
		program_file.close();
	}

	this->vm_context.setup_vm(program_data, base_address);
}

void emulator_t::run_emulator()
{
	this->vm_context.run_vm();
}

void emulator_t::dump_memory(const std::string& dump_name)
{
	if (this->vm_context.initialized)
	{
		this->vm_context.memory_manager->dump_state(dump_name);
	}
	else
		throw std::exception("Attempt to dump memory without VM context initialized!");
}
#include "MM.hpp"

#include <Windows.h>
#include <fstream>

constexpr std::uint8_t& memory_manager_t::read_8(std::size_t index)
{
	if (index >= this->memory_size || index < 0)
		throw std::exception("Attempt to read out of bounds memory");

	return this->block[index];
}

constexpr std::uint16_t memory_manager_t::read_16(std::size_t index)
{
	if (index + 1 >= this->memory_size || index < 0)
		throw std::exception("Attempt to read out of bounds memory");

	return this->read_8(index) | (this->read_8(index + 1) << 8);
}

std::uint8_t& memory_manager_t::read_address_8(std::size_t index)
{
	std::uint16_t address = this->read_16(index);
	if (address >= this->memory_size || address < 0)
		throw std::exception("Address is out of bounds");

	return this->block[address];
}

std::uint16_t memory_manager_t::read_address_16(std::size_t index, std::uint16_t offset)
{
	std::uint16_t address = this->read_16(index);
	if (address >= this->memory_size || address < 0)
		throw std::exception("Address is out of bounds");

	return this->read_16(address + offset);
}

void memory_manager_t::dump_state(const std::string& file_name)
{
	CHAR buffer[MAX_PATH]{ 0 };
	GetCurrentDirectoryA(MAX_PATH, buffer);

	std::string dmp_path{ buffer };
	dmp_path += "\\" + file_name;

	std::ofstream dmp_file{ dmp_path, std::ofstream::out | std::ofstream::binary };
	dmp_file.write(reinterpret_cast<const char*>(this->block), this->memory_size);
	dmp_file.close();

	std::printf("Memory dump written to: %s\n", dmp_path.c_str());
}

void memory_manager_t::load_rom(const std::string& rom_data, std::size_t base_address)
{	
	std::printf("LOADING ROM!\n");

	if (base_address)
	{
		std::size_t end_address = base_address + rom_data.size()-1;

		if (base_address >= this->memory_size || base_address < 0)
			throw std::exception("Attempt to load program in invalid base address");

		if (end_address > this->memory_size)
			throw std::exception("Program is too big to fit in the specified base address");

		std::memcpy(&this->block[base_address], rom_data.c_str(), rom_data.size());
		this->RES = static_cast<std::uint16_t>(base_address);

		std::printf("Entrypoint: 0x%04X\n", this->RES);
	}
	else // Automatically resolves base. Should work on *MOST* programs, but some programs this method won't work on. There isn't really a standard closer than this, so I can't do much about that.
	{
		std::size_t base_load = this->memory_size - rom_data.size() + 1;
		std::printf("Automatically loading into base address: 0x%04X\n", static_cast<std::uint16_t>(base_load));

		std::memcpy(&this->block[base_load], rom_data.c_str(), rom_data.size());

		std::printf("Entrypoint: 0x%04X\n", this->RES);
	}
}
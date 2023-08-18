#include "MM.hpp"
#include <Windows.h>
#include <fstream>

constexpr std::uint8_t& memory_manager_t::read_8(std::size_t index)
{
	if (index >= this->memory_size || index < 0)
		throw std::exception("Attempt to read out of bounds memory");

	return this->memory_block[index];
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

	return this->memory_block[address];
}

std::uint16_t memory_manager_t::read_address_16(std::size_t index)
{
	std::uint16_t address = this->read_16(index);
	if (address >= this->memory_size || address < 0)
		throw std::exception("Address is out of bounds");

	return this->read_16(address);
}

void memory_manager_t::dump_state(const std::string& file_name)
{
	CHAR buffer[MAX_PATH]{ 0 };
	GetCurrentDirectoryA(MAX_PATH, buffer);

	std::string dmp_path{ buffer };
	dmp_path += "\\" + file_name;

	std::ofstream dmp_file{ dmp_path, std::ofstream::out | std::ofstream::binary };
	dmp_file.write(reinterpret_cast<const char*>(this->memory_block), this->memory_size);
	dmp_file.close();

	std::printf("Memory dump written to: %s\n", dmp_path.c_str());
}
#include "emulator/emulator.hpp"

#include <iostream>
#include <memory>
#include <vector>
#include <Windows.h>

int main(int argc, char* argv[])
{
	SetConsoleTitleA("SpectralEmu | Written by Birdy");
	std::printf("Welcome to the SpectralEmu(lator), this is the emulator for the Spectral project!\n");
	std::printf("Load any 6502 machine code into this baby and it will do it's magic.\n\n");

	// Here if you wanna load raw code, don't forget to specify a correct entrypoint and make sure your code corresponds with it.
	std::vector<std::uint8_t> data = {
		0xA2, 0xFF, 0xA0, 0xFF, 0x96, 0x00, 0xCA, 0x88, 0xD0, 0xFA, 0xA9, 0x69, 0x00

		// 0xA9, 0x48, 0x85, 0x00, 0xA9, 0x65, 0x85, 0x01, 0xA9, 0x6C, 0x85, 0x02, 0xA9, 0x6C, 0x85, 0x03, 0xA9, 0x6F, 0x85, 0x04, 0xA9, 0x20, 0x85, 0x05, 0xA9, 0x53, 0x85, 0x06, 0xA9, 0x70, 0x85, 0x07, 0xA9, 0x65, 0x85, 0x08, 0xA9, 0x63, 0x85, 0x09, 0xA9, 0x74, 0x85, 0x0A, 0xA9, 0x72, 0x85, 0x0B, 0xA9, 0x61, 0x85, 0x0C, 0xA9, 0x6C, 0x85, 0x0D, 0xA9, 0x21, 0x85, 0x0E, 0x00	
	};

	std::string my_script{ data.begin(), data.end() };
	try
	{
		std::unique_ptr<emulator_t> emulator = std::make_unique<emulator_t>();
		emulator->load_emulator(my_script, 0x600); // For small programs which don't have a real structure 
		// emulator->load_emulator_path("C:\\Users\\YOURNAME\\SOMEFILE.bin"); // For bin files, usually ~4kb which hold an entire program and interrupts and shit. If it's configured wrong you will have to specify a base address.
		emulator->run_emulator();
		emulator->dump_memory("my_dump.bin"); // Dump memory to a file for analysis with a hex editor, very helpful to see wtf is going on.
	}
	catch (std::exception& except)
	{
		std::printf("An exception has occured while running the emulator!\nException: %s\n", except.what());
	}

	std::printf("Press any key to exit.\n");
	std::cin.get();
	return 0;
}
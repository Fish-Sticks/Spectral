#include <iostream>
#include <Windows.h>
#include <string>

#include "ccompiler/ccompiler.hpp"

int main(int argc, char* argv[])
{
	SetConsoleTitleA("SpectralCompiler | Written by Birdy");
	std::printf("Welcome to the SpectralCompiler, this is the compiler for the Spectral project!\n");
	std::printf("Place any luau code into this baby and it will output 6502 machine instructions.\n\n");

	std::printf("Type a script for it to be compiled, type \"exit\" to close this program.\n> ");

	std::string input{};
	while (std::getline(std::cin, input))
	{
		if (input == "exit")
			break;

		std::string output = cross_compiler::compile_6502(input);
		std::printf("Output: %s\n", output.c_str());

		std::printf("> ");
	}

	std::printf("Press any key to exit.\n");
	std::cin.get();
	return 0;
}
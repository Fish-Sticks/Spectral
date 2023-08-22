#pragma once
#include <string>
#include <Luau/Compiler.h>

namespace cross_compiler
{
	// Takes luau and spits out 6502 code
	std::string compile_6502(const std::string& input_script);
}
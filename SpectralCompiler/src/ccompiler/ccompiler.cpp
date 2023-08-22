#include "ccompiler.hpp"

namespace cross_compiler
{
	// Takes luau and spits out 6502 code
	std::string compile_6502(const std::string& input_script)
	{
		Luau::CompileOptions options{};
		options.coverageLevel = 0;
		options.debugLevel = 0;
		options.optimizationLevel = 2;

		std::string compiled = Luau::compile(input_script, options);


		return compiled;
	}
}
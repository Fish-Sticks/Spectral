#pragma once
#include <cstdint>
#include <unordered_map>

// This is for helping with machine code translation for my emulators virtual machine.
namespace information
{
	enum opcode_t : std::uint8_t
	{
		NONE, // Placeholder
		ADC, AND, ASL, BBR, BBS, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRA, BRK, BVC, BVS, CLC, CLD, CLI, CLV, CMP, CPX, 
		CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP, JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PHX, PHY, PLA, PLP, 
		PLX, PLY, RMB, ROL, ROR, RTI, RTS, SBC, SEC, SED, SEI, SMB, STA, STP, STX, STY, STZ, TAX, TAY, TRB, TSB, TSX, 
		TXA, TXS, TYA, WAI
	};

	/*
		Flags:
		A	Accumulator					OPC A	operand is AC (implied single byte instruction)
		abs	absolute					OPC $LLHH	operand is address $HHLL *
		abs,X	absolute, X-indexed		OPC $LLHH,X	operand is address; effective address is address incremented by X with carry **
		abs,Y	absolute, Y-indexed		OPC $LLHH,Y	operand is address; effective address is address incremented by Y with carry **
		#	immediate					OPC #$BB	operand is byte BB
		impl	implied					OPC	operand implied
		ind	indirect					OPC ($LLHH)	operand is address; effective address is contents of word at address: C.w($HHLL)
		X,ind	X-indexed, indirect		OPC ($LL,X)	operand is zeropage address; effective address is word in (LL + X, LL + X + 1), inc. without carry: C.w($00LL + X)
		ind,Y	indirect, Y-indexed		OPC ($LL),Y	operand is zeropage address; effective address is word in (LL, LL + 1) incremented by Y with carry: C.w($00LL) + Y
		rel	relative					OPC $BB	branch target is PC + signed offset BB ***
		zpg	zeropage					OPC $LL	operand is zeropage address (hi-byte is zero, address = $00LL)
		zpg,X	zeropage, X-indexed		OPC $LL,X	operand is zeropage address; effective address is address incremented by X without carry **
		zpg,Y	zeropage, Y-indexed		OPC $LL,Y	operand is zeropage address; effective address is address incremented by Y without carry **
	*/

	enum flag_t : std::uint8_t
	{
		none,			// Placeholder
		acc,			// Accumulator A
		abs,			// Absolute a 
		abs_X_idx,		// Absolute Indexed with X a,x
		abs_Y_idx,		// Absolute Indexed with Y a,y
		imm,			// Immediate #
		impl,			// Implied i
		ind,			// Indirect
		X_idx_ind,		// X-indexed, indirect
		ind_Y_idx,		// indirect, Y-indexed
		rel,			// Relative
		zp,				// Zero Page zp
		zp_idx_X,		// Zero Page Indexed with X zp,x
		zp_idx_Y,		// Zero Page Indexed with Y zp,y
	};

	struct instruction_t
	{
		instruction_t() {};
		instruction_t(opcode_t op, flag_t flag, const char* name) : op{ op }, flag{ flag }, name{ name } {};
		opcode_t op = opcode_t::NONE;
		flag_t flag = flag_t::none;
		const char* name = nullptr;
	};

	// Every "legal" instruction (illegal instructions AREN'T POSSIBLE ON THIS CPU.)
	// Yes, this should be a switch case on the enum instead of manually encoding every string in the struct, I will do that later once the bigger stuff is done.
	std::unordered_map<std::uint8_t, const instruction_t> opcode_map =
	{
		{0x00, {BRK, impl, "BRK"}}, {0x01, {ORA, X_idx_ind, "ORA"}}, {0x05, {ORA, zp, "ORA"}}, {0x06, {ASL, zp, "ASL"}}, {0x08, {PHP, impl, "PHP"}}, {0x09, {ORA, imm, "ORA"}}, {0x0A, {ASL, acc, "ASL"}}, {0x0D, {ORA, abs, "ORA"}}, {0x0E, {ASL, abs, "ASL"}}, {0x10, {BPL, rel, "BPL"}}, {0x11, {ORA, ind_Y_idx, "ORA"}},
		{0x15, {ORA, zp_idx_X, "ORA"}}, {0x16, {ASL, zp_idx_X, "ASL"}}, {0x18, {CLC, impl, "CLC"}}, {0x19, {ORA, abs_Y_idx, "ORA"}}, {0x1D, {ORA, abs_X_idx, "ORA"}}, {0x1E, {ASL, abs_X_idx, "ASL"}}, {0x20, {JSR, abs, "JSR"}}, {0x21, {AND, X_idx_ind, "AND"}}, {0x24, {BIT, zp, "BIT"}}, {0x25, {AND, zp, "AND"}},
		{0x26, {ROL, zp, "ROL"}}, {0x28, {PLP, impl, "PLP"}}, {0x29, {AND, imm, "AND"}}, {0x2A, {ROL, acc, "ROL"}}, {0x2C, {BIT, abs, "BIT"}}, {0x2D, {AND, abs, "AND"}}, {0x2E, {ROL, abs, "ROL"}}, {0x30, {BMI, rel, "BMI"}}, {0x31, {AND, ind_Y_idx, "AND"}}, {0x35, {AND, zp_idx_X, "AND"}},
		{0x36, {ROL, zp_idx_X, "ROL"}}, {0x38, {SEC, impl, "SEC"}}, {0x39, {AND, abs_Y_idx, "AND"}}, {0x3D, {AND, abs_X_idx, "AND"}}, {0x3E, {ROL, abs_X_idx, "ROL"}}, {0x40, {RTI, impl, "RTI"}}, {0x41, {EOR, X_idx_ind, "EOR"}}, {0x45, {EOR, zp, "EOR"}}, {0x46, {LSR, zp, "LSR"}}, {0x48, {PHA, impl, "PHA"}},
		{0x49, {EOR, imm, "EOR"}}, {0x4A, {LSR, acc, "LSR"}}, {0x4C, {JMP, abs, "JMP"}}, {0x4D, {EOR, abs, "EOR"}}, {0x4E, {LSR, abs, "LSR"}}, {0x50, {BVC, rel, "BVC"}}, {0x51, {EOR, ind_Y_idx, "EOR"}}, {0x55, {EOR, zp_idx_X, "EOR"}}, {0x56, {LSR, zp_idx_X, "LSR"}}, {0x58, {CLI, impl, "CLI"}},
		{0x59, {EOR, abs_Y_idx, "EOR"}}, {0x5D, {EOR, abs_X_idx, "EOR"}}, {0x5E, {LSR, abs_X_idx, "LSR"}}, {0x60, {RTS, impl, "RTS"}}, {0x61, {ADC, X_idx_ind, "ADC"}}, {0x65, {ADC, zp, "ADC"}}, {0x66, {ROR, zp, "ROR"}}, {0x68, {PLA, impl, "PLA"}}, {0x69, {ADC, imm, "ADC"}}, {0x6A, {ROR, acc, "ROR"}},
		{0x6C, {JMP, ind, "JMP"}}, {0x6D, {ADC, abs, "ADC"}}, {0x6E, {ROR, abs, "ROR"}}, {0x70, {BVS, rel, "BVS"}}, {0x71, {ADC, ind_Y_idx, "ADC"}}, {0x75, {ADC, zp_idx_X, "ADC"}}, {0x76, {ROR, zp_idx_X, "ROR"}}, {0x78, {SEI, impl, "SEI"}}, {0x79, {ADC, abs_Y_idx, "ADC"}}, {0x7D, {ADC, abs_X_idx, "ADC"}},
		{0x7E, {ROR, abs_X_idx, "ROR"}}, {0x81, {STA, X_idx_ind, "STA"}}, {0x84, {STY, zp, "STY"}}, {0x85, {STA, zp, "STA"}}, {0x86, {STX, zp, "STX"}}, {0x88, {DEY, impl, "DEY"}}, {0x8A, {TXA, impl, "TXA"}}, {0x8C, {STY, abs, "STY"}}, {0x8D, {STA, abs, "STA"}}, {0x8E, {STX, abs, "STX"}},
		{0x90, {BCC, rel, "BCC"}}, {0x91, {STA, ind_Y_idx, "STA"}}, {0x94, {STY, zp_idx_X, "STY"}}, {0x95, {STA, zp_idx_X, "STA"}}, {0x96, {STX, zp_idx_Y, "STX"}}, {0x98, {TYA, impl, "TYA"}}, {0x99, {STA, abs_Y_idx, "STA"}}, {0x9A, {TXS, impl, "TXS"}}, {0x9D, {STA, abs_X_idx, "STA"}}, {0xA0, {LDY, imm, "LDY"}},
		{0xA1, {LDA, X_idx_ind, "LDA"}}, {0xA2, {LDX, imm, "LDX"}}, {0xA4, {LDY, zp, "LDY"}}, {0xA5, {LDA, zp, "LDA"}}, {0xA6, {LDX, zp, "LDX"}}, {0xA8, {TAY, impl, "TAY"}}, {0xA9, {LDA, imm, "LDA"}}, {0xAA, {TAX, impl, "TAX"}}, {0xAC, {LDY, abs, "LDY"}}, {0xAD, {LDA, abs, "LDA"}},
		{0xAE, {LDX, abs, "LDX"}}, {0xB0, {BCS, rel, "BCS"}}, {0xB1, {LDA, ind_Y_idx, "LDA"}}, {0xB4, {LDY, zp_idx_X, "LDY"}}, {0xB5, {LDA, zp_idx_X, "LDA"}}, {0xB6, {LDX, zp_idx_Y, "LDX"}}, {0xB8, {CLV, impl, "CLV"}}, {0xB9, {LDA, abs_Y_idx, "LDA"}}, {0xBA, {TSX, impl, "TSX"}}, {0xBC, {LDY, abs_X_idx, "LDY"}},
		{0xBD, {LDA, abs_X_idx, "LDA"}}, {0xBE, {LDX, abs_Y_idx, "LDX"}}, {0xC0, {CPY, imm, "CPY"}}, {0xC1, {CMP, X_idx_ind, "CMP"}}, {0xC4, {CPY, zp, "CPY"}}, {0xC5, {CMP, zp, "CMP"}}, {0xC6, {DEC, zp, "DEC"}}, {0xC8, {INY, impl, "INY"}}, {0xC9, {CMP, imm, "CMP"}}, {0xCA, {DEX, impl, "DEX"}},
		{0xCC, {CPY, abs, "CPY"}}, {0xCD, {CMP, abs, "CMP"}}, {0xCE, {DEC, abs, "DEC"}}, {0xD0, {BNE, rel, "BNE"}}, {0xD1, {CMP, ind_Y_idx, "CMP"}}, {0xD5, {CMP, zp_idx_X, "CMP"}}, {0xD6, {DEC, zp_idx_X, "DEC"}}, {0xD8, {CLD, impl, "CLD"}}, {0xD9, {CMP, abs_Y_idx, "CMP"}}, {0xDD, {CMP, abs_X_idx, "CMP"}},
		{0xDE, {DEC, abs_X_idx, "DEC"}}, {0xE0, {CPX, imm, "CPX"}}, {0xE1, {SBC, X_idx_ind, "SBC"}}, {0xE4, {CPX, zp, "CPX"}}, {0xE5, {SBC, zp, "SBC"}}, {0xE6, {INC, zp, "INC"}}, {0xE8, {INX, impl, "INX"}}, {0xE9, {SBC, imm, "SBC"}}, {0xEA, {NOP, impl, "NOP"}}, {0xEC, {CPX, abs, "CPX"}},
		{0xED, {SBC, abs, "SBC"}}, {0xEE, {INC, abs, "INC"}}, {0xF0, {BEQ, rel, "BEQ"}}, {0xF1, {SBC, ind_Y_idx, "SBC"}}, {0xF5, {SBC, zp_idx_X, "SBC"}}, {0xF6, {INC, zp_idx_X, "INC"}}, {0xF8, {SED, impl, "SED"}}, {0xF9, {SBC, abs_Y_idx, "SBC"}}, {0xFD, {SBC, abs_X_idx, "SBC"}}, {0xFE, {INC, abs_X_idx, "INC"}}
	};
}


/*
	Website: https://www.masswerk.at/6502/6502_instruction_set.html
	Website dump script that I made to auto generate opcode mappings:

	// Must modify the HTML element to have a tag so javascript can access it
	let children = document.getElementById("billy").children
	let dumped_list = []
	let replace_list = {
		"A": "acc",
		"abs": "abs",
		"abs,X": "abs_X_idx",
		"abs,Y": "abs_Y_idx",
		"#": "imm",
		"impl": "impl",
		"ind": "ind",
		"X,ind": "X_idx_ind",
		"ind,Y": "ind_Y_idx",
		"rel": "rel",
		"zpg": "zp",
		"zpg,X": "zp_idx_X",
		"zpg,Y": "zp_idx_Y"
	}

	// Dump into neat list
	for (let x = 0; x < children.length; ++x)
	{
		let current_node = children[x]
		let current_children = current_node.children
		let current_row = current_children[0].innerText

		for (let y = 1; y < current_children.length; ++y)
		{
			let current_element = current_children[y]
			if (current_element.innerText === "---")
				continue

			let split = current_element.innerText.split(' ')
			let instruction = split[0]
			let new_thing = replace_list[split[1]]
			let hex = ("0x" + current_row + (Number(y) - 1).toString(16).toUpperCase()).replace('‐', '')
			dumped_list[dumped_list.length] = `{${hex}, {${instruction}, ${new_thing}, "${instruction}"}}`
		}
	}

	let output_string = ""
	for (let i = 0; i < dumped_list.length; ++i)
	{
		output_string += dumped_list[i] + ", "
		if (i % 10 == 0)
			output_string += "\n"
	}

	console.log(output_string)
*/
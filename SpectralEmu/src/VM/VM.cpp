#include "VM.hpp"
#include "6502/logic.hpp"

void vm_context_t::setup_vm(const std::string& machine_code)
{
	this->machine_code = machine_code;
	this->memory_block.resize(0xFFFF, 0);
	this->memory_manager = std::make_unique<memory_manager_t>();
	this->initialized = true;
}

void vm_context_t::clear_vm()
{
	this->machine_code.clear();
	this->memory_block.clear();

	this->PC = 0;
	this->SR = 0;
	this->AC = 0;
	this->X = 0;
	this->Y = 0;
	this->SP = 0;
	this->initialized = false;
}

void vm_context_t::dump_ctx()
{
	auto resolve = [](bool input) -> const char*
	{
		return input ? "true" : "false";
	};

	std::printf("\n\tVM CONTEXT DUMP\t\n");
	std::printf("PC: 0x%04X\tSP: 0x%02X\tACC: 0x%02X\n", this->PC, this->SP, static_cast<unsigned char>(this->AC));
	std::printf("X: 0x%02X\t\tY: 0x%02X\n", static_cast<unsigned char>(this->X), static_cast<unsigned char>(this->Y));
	
	std::printf("SR: 0x%02X\n", this->SR);
	std::printf("SR flags:\n");
	std::printf("Carry: %s\tZero: %s\t\tInterrupt: %s\tDecimal: %s\nBreak: %s\tOverflow: %s\t\tNegative: %s\n\n",
		resolve(this->SR_flags.C), resolve(this->SR_flags.Z), resolve(this->SR_flags.I), resolve(this->SR_flags.D),
		resolve(this->SR_flags.B), resolve(this->SR_flags.V), resolve(this->SR_flags.N));
}

void vm_context_t::set_comparison_flags(std::uint8_t compared_against, std::uint8_t operand)
{
	std::uint8_t result = compared_against - operand;
	this->SR_flags.N = result & (1 << 7);

	if (compared_against < operand || compared_against > operand)
	{
		this->SR_flags.Z = false;
		this->SR_flags.C = (compared_against > operand ? true : false);
	}
	else if (compared_against == operand)
	{
		this->SR_flags.Z = true;
		this->SR_flags.C = true;
		this->SR_flags.N = false;
	}
}

// TODO: Convert over from a "code" stream into a pre-computed memory space just like how it is for real this way programs are truly running themselves.
// Right now it's just "faking it" by reading a data stream with no address access.
void vm_context_t::run_vm()
{
	using namespace information;

	if (!this->initialized)
		throw std::exception("The VM is not initialized!");

	std::string code = this->machine_code;

	while (true)
	{
		static int instructions_ran = 0;
		++instructions_ran;

		if (this->SR_flags.B)
		{
			std::printf("Interrupt break, exiting VM!\n");
			std::printf("Ran %d instructions.\n", instructions_ran);
			break;
		}

		const instruction_t current_ins = opcode_map[code[this->PC++]]; // Increment PC and get current instruction
		std::uint16_t operand = 0; // Copy value which is to be written back, 16 bits because it CAN be an address too, operations on registers will ALWAYS be 8 bits so no worry there.

		// Decode operand, the meaning of this is in 6502/logic.hpp
		// Problem: std::string is using signed data, i might need make my own datatype so data is handled correctly.
		switch (current_ins.flag)
		{
			case acc:
				std::printf("ACC triggered but not implemented!\n");
				break;
			case flag_t::abs: // fuck STL
				operand = code[this->PC++] | (code[this->PC++] << 8);
				break;
			case abs_X_idx:
				operand = code[this->PC++] | (code[this->PC++] << 8) + this->X;
				break;
			case abs_Y_idx:
				operand = code[this->PC++] | (code[this->PC++] << 8) + this->Y;
				break;
			case imm:
				operand = static_cast<std::uint8_t>(code[this->PC++]);
				break;
			case impl:
				operand = -1; // "-1" = IMPLICIT MODE ( helps for debugging hence why not 0, operand won't be used anyways ) 
				break;
			case ind:
			{
				std::uint16_t address = static_cast<std::uint8_t>(code[this->PC++]) | static_cast<std::uint8_t>((code[this->PC++] << 8));
				operand = *reinterpret_cast<std::uint16_t*>(&this->memory_block[address]);
				break;
			}
			case X_idx_ind:
				std::printf("X_idx_ind triggered but not implemented!\n");
				break;
			case ind_Y_idx:
				std::printf("ind_Y_idx triggered but not implemented!\n");
				break;
			case rel:
				operand = this->PC + static_cast<signed char>(code[this->PC++]);
				break;
			case zp: // unlike immediate this is 16 bits (for memory reasons ig) but this only sets low byte
				operand = static_cast<std::uint8_t>(code[this->PC++]);
				break;
			case zp_idx_X:
				std::printf("zp_idx_X triggered but not implemented!\n");
				break;
			case zp_idx_Y:
				std::printf("zp_idx_Y triggered but not implemented!\n");
				break;
			default:
				std::printf("Invalid instruction flag: %d\n", current_ins.flag);
				this->SR_flags.B = true;
				break;
		}

		std::printf("Executing instruction: %s | Operand: 0x%04X\n", current_ins.name, static_cast<std::uint16_t>(operand));

		switch (current_ins.op)
		{
		// Accumulator operations
		case CMP:
			this->set_comparison_flags(this->AC, operand);
			break;
		case STA:
			this->memory_block[operand] = this->AC;
			break;
		case LDA:
			this->AC = static_cast<std::uint8_t>(operand);
			break;
		case TXA:
			this->AC = this->X;
			break;
		case TYA:
			this->AC = this->Y;
			break;
		case ADC:
		{
			std::uint8_t old_ac = this->AC; // used for overflow check
			this->AC += operand;
			break;
		}

		// X operations
		case CPX:
			this->set_comparison_flags(this->X, operand);
			break;
		case STX:
			this->memory_block[operand] = this->X;
			break;
		case LDX:
			this->X = static_cast<std::uint8_t>(operand);
			break;
		case TAX:
			this->X = this->AC;
			break;
		case TSX:
			this->X = this->SP;
			break;
		case TXS:
			this->SP = this->X;
			break;
		case INX:
			this->X++;
			break;
		case DEX:
			this->X--;
			break;

		// Y operations
		case CPY:
			this->set_comparison_flags(this->Y, operand);
			break;
		case STY:
			this->memory_block[operand] = this->Y;
			break;
		case LDY:
			this->Y = static_cast<std::uint8_t>(operand);
			break;
		case TAY:
			this->Y = this->AC;
			break;
		case INY:
			this->Y++;
			break;
		case DEY:
			this->Y--;
			break;

		// CF operations
		case JMP:
			this->PC = operand;
			break;

		// Branch operations
		case BNE:
			this->PC = (this->SR_flags.Z ? this->PC : operand);
			break;
		case BEQ:
			this->PC = (this->SR_flags.Z ? operand : this->PC);
			break;
		case BCC:
			this->PC = (this->SR_flags.C ? this->PC : operand);
			break;
		case BCS:
			this->PC = (this->SR_flags.C ? operand : this->PC);
			break;
		case BPL:
			this->PC = (this->SR_flags.N ? this->PC : operand);
			break;
		case BMI:
			this->PC = (this->SR_flags.N ? operand : this->PC);
			break;
		case BVC:
			this->PC = (this->SR_flags.V ? this->PC : operand);
			break;
		case BVS:
			this->PC = (this->SR_flags.V ? operand : this->PC);
			break;

		// Exit operations
		case BRK: // IMPLEMENT INTERRUPTS LATER.
			this->SR_flags.B = true;
			break;
		case RTS:
			this->SR_flags.B = true; // Quit the VM
			break;
		default:
			std::printf("Invalid instruction ran: 0x%02X\n", current_ins.op);
			this->SR_flags.B = true;
			break;
		}

		continue;
	}

	std::printf("Program finished executing.\n");
	this->dump_ctx();
}
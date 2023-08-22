#include "VM.hpp"
#include "6502/logic.hpp"

void vm_context_t::setup_vm(const std::string& machine_code, std::size_t base_address)
{
	this->memory_manager = std::make_unique<memory_manager_t>();
	this->memory_manager->load_rom(machine_code, base_address);
	this->SP = 0xFF;
	this->PC = this->memory_manager->RES;
	this->initialized = true;
}

void vm_context_t::clear_vm()
{
	this->PC = 0;
	this->SR = 0;
	this->SP = 0;
	this->AC = 0;
	this->X = 0;
	this->Y = 0;
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
	this->SR_flags.N = (result >> 7) & 1;

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

void vm_context_t::set_arith_flags(std::uint8_t result)
{
	this->SR_flags.Z = result == 0;
	this->SR_flags.N = (result >> 7) & 1;
}

void vm_context_t::run_vm()
{
	using namespace information;

	if (!this->initialized)
		throw std::exception("The VM is not initialized!");
	
	memory_manager_t& memory = *this->memory_manager;

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

		std::uint16_t original_PC = this->PC;
		const instruction_t current_ins = opcode_map[memory[this->PC++]]; // Increment PC and get current instruction
		std::uint16_t operand = 0; // Copy value which is to be written back, 16 bits because it CAN be an address too, operations on registers will ALWAYS be 8 bits so no worry there.
		bool is_op_acc = false;

		// Decode operand, the meaning of this is in 6502/logic.hpp
		switch (current_ins.flag)
		{
			case acc:
				is_op_acc = true;
				break;
			case flag_t::abs: // fuck STL
				operand = memory.read_16(this->PC++);
				this->PC++;
				break;
			case abs_X_idx:
				operand = memory.read_16(this->PC++) + this->X;
				this->PC++;
				break;
			case abs_Y_idx:
				operand = memory.read_16(this->PC++) + this->Y;
				this->PC++;
				break;
			case imm:
				operand = memory[this->PC++];
				break;
			case impl:
				operand = -1; // "-1" = IMPLICIT MODE ( helps for debugging hence why not 0, operand won't be used anyways ) 
				break;
			case ind:
			{
				operand = memory.read_address_16(this->PC++);
				this->PC++;
				break;
			}
			case X_idx_ind:
				operand = memory.read_address_16(memory[this->PC++] + this->X);
				break;
			case ind_Y_idx:
				operand = memory.read_address_16(memory[this->PC++], this->Y);
				break;
			case rel:
				operand = this->PC + static_cast<std::int8_t>(memory[this->PC++]); // +-127 bytes
				break;
			case zp: // unlike immediate this is 16 bits (for memory reasons ig) but this only sets low byte
				operand = memory[this->PC++];
				break;
			case zp_idx_X:
				operand = memory[this->PC++] + this->X;
				break;
			case zp_idx_Y:
				operand = memory[this->PC++] + this->Y;
				break;
			default:
				std::printf("Invalid instruction flag: %d\n", current_ins.flag);
				this->SR_flags.B = true;
				break;
		}

		std::printf("[0x%04X] | Executing instruction: %s | Operand: 0x%04X\n", original_PC, current_ins.name, static_cast<std::uint16_t>(operand));

		switch (current_ins.op)
		{
		// Accumulator operations
		case CMP:
			this->set_comparison_flags(this->AC, static_cast<std::uint8_t>(operand));
			break;
		case STA:
			memory[operand] = this->AC;
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
			this->AC += operand;
			break;
		case SBC:
			this->AC -= operand;
			break;

		// X operations
		case CPX:
			this->set_comparison_flags(this->X, static_cast<std::uint8_t>(operand));
			break;
		case STX:
			memory[operand] = this->X;
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
			this->set_arith_flags(this->X++);
			break;
		case DEX:
			this->set_arith_flags(this->X--);
			break;

		// Y operations
		case CPY:
			this->set_comparison_flags(this->Y, static_cast<std::uint8_t>(operand));
			break;
		case STY:
			memory[operand] = this->Y;
			break;
		case LDY:
			this->Y = static_cast<std::uint8_t>(operand);
			break;
		case TAY:
			this->Y = this->AC;
			break;
		case INY:
			this->set_arith_flags(this->Y++);
			break;
		case DEY:
			this->set_arith_flags(this->Y--);
			break;

		// Stack operations
		case PHA:
			memory.stack[this->SP--] = this->AC;
			break;
		case PLA:
			this->AC = memory.stack[++this->SP];
			break;
		case PHP:
			memory.stack[this->SP--] = this->SR | (1 << 4) | (1 << 5); // with break flag and bit 5 set, idk why yet... fix later
			break;
		case PLP:
			this->SR = memory.stack[++this->SP] & ~((1 << 4) | (1 << 5)); // with break flag and bit 5 ignored
			break;

		// logical operations
		case AND:
			this->AC &= operand;
			break;
		case EOR:
			this->AC ^= operand;
			break;
		case ORA:
			this->AC |= operand;
			break;
		case ASL:
			if (is_op_acc)
				this->AC <<= 1;
			else
				memory[operand] <<= 1;
			break;
		case LSR:
			if (is_op_acc)
				this->AC >>= 1;
			else
				memory[operand] >>= 1;
			break;
		case ROL:
		{
			std::uint8_t old_carry = this->SR_flags.C;
			if (is_op_acc)
			{
				this->SR_flags.C = (this->AC >> 7) & 1;
				this->AC = (this->AC << 1) | old_carry;
			}
			else
			{
				this->SR_flags.C = (memory[operand] >> 7) & 1;
				memory[operand] = (memory[operand] << 1) | old_carry;
			}
			break;
		}
		case ROR:
		{
			std::uint8_t old_carry = this->SR_flags.C;
			if (is_op_acc)
			{
				this->SR_flags.C = this->AC & 1;
				this->AC = (this->AC >> 1) | (old_carry << 7);
			}
			else
			{
				this->SR_flags.C = memory[operand] & 1;
				memory[operand] = (memory[operand] >> 1) | (old_carry << 7);
			}
			break;
		}
		case INC:
			++memory[operand];
			break;
		case DEC:
			--memory[operand];
			break;

		// Funky flags
		case CLC:
			this->SR_flags.C = false;
			break;
		case CLD:
			this->SR_flags.D = false;
			break;
		case CLI:
			this->SR_flags.I = false;
			break;
		case CLV:
			this->SR_flags.V = false;
			break;
		case SEC:
			this->SR_flags.C = true;
			break;
		case SED:
			this->SR_flags.D = true;
			break;
		case SEI:
			this->SR_flags.I = true;
			break;

		// JMPS & SUBROUTINES
		case JMP:
			this->PC = operand;
			break;
		case JSR: // NOTE: PC is already at the next instruction.
			memory.stack[this->SP--] = static_cast<std::uint8_t>(this->PC >> 8); // High byte
			memory.stack[this->SP--] = static_cast<std::uint8_t>(this->PC); // Low byte
			this->PC = operand;
			break;
		case RTS:
			this->PC = memory.stack[++this->SP] | (memory.stack[++this->SP] << 8);
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

		// Interrupts and misc
		case BRK: // IMPLEMENT INTERRUPTS LATER.
			this->SR_flags.B = true;
			break;
		case RTI:
			std::printf("RTI isn't implemented yet!\n");
			break;
		case NOP:
			// NOPE.
			break;
		case BIT:
			this->SR_flags.V = (operand >> 6) & 1;
			this->SR_flags.N = (operand >> 7) & 1;
			this->SR_flags.Z = (this->AC & operand) == 0;
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
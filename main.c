#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#define DEBUG 1
#define dprintf(...) if(DEBUG) printf(__VA_ARGS__)

#define STACK_SIZE 1024*3 // 3Mb.
#define NUM_REGS 4
typedef uint64_t INT;

enum opcodes {
  NOP = 0x00,
  MOV = 0x01,
  PUSH = 0x02,
  POP = 0x03,
  CMP = 0x04,
  CALL = 0x05,
  RET = 0x06,
  JMP = 0x07,
  JNE = 0x08,
  JEQ = 0x09,
  JGT = 0x0A,
  JLT = 0x0B,
  JGE = 0x0C,
  JLE = 0x0D,
  INC = 0x0E,
  DEC = 0x0F,
  ADD = 0x10,
  SUB = 0x11,
  MULT = 0x12,
  DIV = 0x13,
  PRINT = 0x14,
  HPRNT = 0x15,
};
enum flag_registers {
  COMPARE = 0x00
};
enum compare_result_types {
  EQUAL_TO = 0x01,
  GREATER_THAN = 0x02,
  LESS_THAN = 0x03,
  GREATER_THAN_OR_EQUAL = 0x04,
  LESS_THAN_OR_EQUAL = 0x05
};
enum value_types {
  INTEGER = 0x00,
  DECIMAL = 0x01,
  STRING = 0x02,
  OBJECT = 0x03
};

INT pc = 0;
INT sp = 0;
INT registers[NUM_REGS];
INT flags[4];

INT stack[STACK_SIZE];

/////////////////////////////////
/* The mother fucking program! */
/////////////////////////////////
INT program[] = {
                  // Save these values for whatever reason.
                  MOV, 0x00, 0xDEAD,
                  MOV, 0x01, 0x0BED,
                  PUSH, 0x00,
                  PUSH, 0x01,

                  // Set register[0] to 0.
                  MOV, 0x00, 0x00,
                  HPRNT, 0x00, // This prints HEX to the terminal.

                  // Set register[1] to 32.
                  MOV, 0x01, 0x20,
                  HPRNT, 0x01,

                  // Jump back to here unless reg[0] > reg[1]!
                  ADD, 0x00, 0x08,
                  HPRNT, 0x00,
                  CMP, // Compare the first 2 registers and set a flag.
                  JLT, 0x14, // Jumping...

                  // Get the original register values back!
                  POP, 0x01,
                  POP, 0x00,
                  // Get out of here.
                  RET,
                };

  int main(int argc, char** argv)
  {
    INT program_size = (sizeof(program) / sizeof(INT))-1;
    dprintf("=========================\n");
    dprintf("Program Size: %lld bytes!\n\n", program_size*8);
    dprintf("Running program...\n");
    dprintf("=========================\n\n");

    do
    {
      switch(program[pc])
      {
        case INC:
          registers[program[pc+1]] += 1;
          ++pc;
        break;

        case DEC:
          registers[program[pc+1]] -= 1;
          ++pc;
        break;

        case ADD:
          registers[program[pc+1]] += program[pc+2];
          pc += 3;
        break;

        case SUB:
          registers[program[pc+1]] -= program[pc+2];
          pc += 3;
        break;

        case MOV:
          registers[program[pc+1]] = program[pc+2];
          dprintf("MOVED %llX to register[%llX]\n", program[pc+2], program[pc+1]);
          pc += 3;
        break;

        case PUSH:
          stack[sp] = registers[program[pc+1]];
          dprintf("Pushed value in register[%lld] to the stack\n", program[pc+1]);
          ++sp;
          pc += 2;
        break;

        case POP:
          registers[program[pc+1]] = stack[sp-1];
          dprintf("Popped stack top to register[%lld]\n", program[pc+1]);
          --sp;
          pc += 2;
        break;

        case PRINT:
          printf("%lld\n", registers[program[pc+1]]);
          pc += 2;
        break;
        case HPRNT:
          printf("0x%llX\n", registers[program[pc+1]]);
          pc += 2;
        break;

        case JMP:
          dprintf("JMP'd to address: %llX\n", program[pc+1]);
          pc = program[pc+1];
        break;

        case CMP:
          if(registers[0] == registers[1])
          {
            flags[COMPARE] = EQUAL_TO;
          }
          else if(registers[0] > registers[1])
          {
            flags[COMPARE] = GREATER_THAN;
          }
          else if(registers[0] < registers[1])
          {
            flags[COMPARE] = LESS_THAN;
          }
          else if(registers[0] >= registers[1])
          {
            flags[COMPARE] = GREATER_THAN_OR_EQUAL;
          }
          else if(registers[0] <= registers[1])
          {
            flags[COMPARE] = LESS_THAN_OR_EQUAL;
          }
          dprintf("Compared %llX to %llX \n", registers[0], registers[1]);
          ++pc;
        break;

        case JLT:
          if(flags[COMPARE] == LESS_THAN)
          {
            dprintf("JMP'd to address: %llX\n", program[pc+1]);
            pc = program[pc+1];
          }
          else
          {
            pc += 2;
          }
        break;

        case JLE:
          if(flags[COMPARE] == LESS_THAN_OR_EQUAL)
          {
            dprintf("JMP'd to address: %llX\n", program[pc+1]);
            pc = program[pc+1];
          }
          else
          {
            pc += 2;
          }
        break;

        case JGT:
          if(flags[COMPARE] == GREATER_THAN)
          {
            dprintf("JMP'd to address: %llX\n", program[pc+1]);
            pc = program[pc+1];
          }
          else
          {
            pc += 2;
          }
        break;

        case JGE:
          if(flags[COMPARE] == GREATER_THAN_OR_EQUAL)
          {
            dprintf("JMP'd to address: %llX\n", program[pc+1]);
            pc = program[pc+1];
          }
          else
          {
            pc += 2;
          }
        break;

        case NOP:
          ++pc;
        break;

        case RET:
          goto exit_program;
        break;

        default:
          ++pc;
        break;
      }
      //sleep(1);
      //dprintf("Stack pointer: %lld / Value: %lld\n", sp, stack[sp]);
      //dprintf("Reg: %lld\n", registers[program[pc+1]]);

    } while (pc < program_size);

    exit_program:;

    printf("\nValue of register 0 is: 0x%llX\n", registers[0]);
    printf("Value of register 1 is: 0x%llX\n\n", registers[1]);

    return 0;
  }

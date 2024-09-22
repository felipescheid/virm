#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define VIRM_STACK_CAPACITY 1024

typedef enum {
	TRAP_OK = 0,
	TRAP_STACK_OVERFLOW,
	TRAP_STACK_UNDERFLOW,
	TRAP_ILLEGAL_INST
} Trap;

typedef int64_t Word;

// Virm = virtual machine
typedef struct {
	Word stack[VIRM_STACK_CAPACITY];
	size_t stack_size; //size_t represents the size of objects in memory in bytes
} Virm;

// VM Instructions
typedef enum {
	INST_PUSH,
	INST_PLUS // add two first elements of the stack and push them back onto stack 
} Inst_Type;

typedef struct {
	Inst_Type type;
	Word operand;
} Inst;

Inst inst_push(Word operand)
{
	return (Inst) {
		.type = INST_PUSH,
		.operand = operand
	};
}

Inst inst_plus()
{
	return (Inst) {
		.type = INST_PLUS
	};
}

Trap virm_execute_inst(Virm *virm, Inst inst)
{
	switch (inst.type) {
	// The arrow operator is used to access members of a structure through a pointer
	// The dot operator is used to access members of a structure directly
	case INST_PUSH:
		if (virm->stack_size > VIRM_STACK_CAPACITY) {
			return TRAP_STACK_OVERFLOW;
		}
		virm->stack[virm->stack_size++] = inst.operand;
		break;

	case INST_PLUS:
		if (virm->stack_size < 2) {
			return TRAP_STACK_UNDERFLOW;
		}
		virm->stack[virm->stack_size - 2] += virm->stack[virm->stack_size - 1];
		virm->stack_size -= 1;
		break;

	default:
		return TRAP_ILLEGAL_INST;

	}

	return TRAP_OK;
}

// const specifies that the function can read the virm obj but not change it
void virm_dump(const Virm *virm)
{
	printf("Stack:\n");
	if (virm->stack_size > 0) {
		for (size_t i = 0; i < virm->stack_size; ++i) {
			printf(" %ld\n", virm->stack[i]);
		}
	} else {
		printf("[Empty] \n");
	}
}

Virm virm = {0};

int main()
{
	virm_dump(&virm);
	virm_execute_inst(&virm, inst_push(18));
	virm_dump(&virm);
	virm_execute_inst(&virm, inst_push(18));
	virm_dump(&virm);
	virm_execute_inst(&virm, inst_push(18));
	return 0;
}

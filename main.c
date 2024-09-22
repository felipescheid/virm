#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <inttypes.h>

#define VIRM_STACK_CAPACITY 1024

typedef enum {
	ERR_OK = 0,
	ERR_STACK_OVERFLOW,
	ERR_STACK_UNDERFLOW,
	ERR_ILLEGAL_INST
} Err;

const char *err_as_cstr(Err err)
{
	switch(err) {
	case ERR_OK:
		return "ERR_OK";
	case ERR_STACK_OVERFLOW:
		return "ERR_STACK_OVERFLOW";
	case ERR_STACK_UNDERFLOW:
		return "ERR_STACK_UNDERFLOW";
	case ERR_ILLEGAL_INST:
		return "ERR_ILLEGAL_INST";
	default:
		assert(0 && "err_as_cstr: Unreachable");
	}
}
	

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

#define MAKE_INST_PUSH(value) {.type = INST_PUSH, .operand = (value)}
#define MAKE_INST_PLUS {.type = INST_PLUS}

Err virm_execute_inst(Virm *virm, Inst inst)
{
	switch(inst.type) {
	case INST_PUSH:
		if (virm->stack_size >= VIRM_STACK_CAPACITY) {
			return ERR_STACK_OVERFLOW;
		}
		virm->stack[virm->stack_size++] = inst.operand;
		break;

	case INST_PLUS:
		if (virm->stack_size < 2) {
			return ERR_STACK_UNDERFLOW;
		}
		virm->stack[virm->stack_size - 2] += virm->stack[virm->stack_size - 1];
		virm->stack_size -= 1;
		break;

	default:
		return ERR_ILLEGAL_INST;

	}

	return ERR_OK;
}

// const specifies that the function can read the virm obj but not change it
void virm_dump(FILE *stream, const Virm *virm)
{
	fprintf(stream, "Stack:\n");
	if (virm->stack_size > 0) {
		// the PRId64 macro from <inttypes.h> ensures the correct format specifier for int64_t for the system
		for (size_t i = 0; i < virm->stack_size; ++i) {
			fprintf(stream, " %" PRId64 "\n", virm->stack[i]);
		}
	} else {
		fprintf(stream, " [Empty] \n");
	}
}

// In C, all array elements are stored one after another in memory. Since all elements are of the same type, they will
// have the same number of bites. This is why dividing the total size of the array by the size of the first element
// gives us the number of elements in the array
#define ARRAY_SIZE(xs) (sizeof(xs) / sizeof((xs)[0]))

Virm virm = {0};

Inst program[] = {
	MAKE_INST_PUSH(18),
	MAKE_INST_PUSH(18),
	MAKE_INST_PUSH(18),
	MAKE_INST_PLUS,
};

int main(void)
{
	virm_dump(stdout, &virm);
	for (size_t i = 0; i < ARRAY_SIZE(program); ++i) {
		Err err = virm_execute_inst(&virm, program[i]);
		if (err != ERR_OK) {
			fprintf(stderr, "ERROR: %s\n", err_as_cstr(err));
			virm_dump(stderr, &virm);
			exit(1);
		} 
	}
	virm_dump(stdout, &virm);
	return 0;
}

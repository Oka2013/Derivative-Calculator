#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 256

typedef unsigned char byte;

enum {
    OP_PUSH,
    OP_POP,
    OP_DUP,
    OP_DEL,
    OP_PICK,
    OP_SWAP,
    OP_DONE,

    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,

    OP_POW,
    OP_EXP,
    OP_LOG,
    OP_LN,
    OP_NEG,
    OP_ABS,
    OP_SQRT,
    OP_NQRT,
    OP_SIN,
    OP_COS,
};

typedef enum{
    CH_CONSTANT,
    CH_VARIABLE,
} ChangeType;

byte derivatives[18][3][30] = {
    {
        {OP_DEL, OP_DEL, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f, OP_DONE},
        {OP_DEL, OP_DEL, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f},
        {OP_DEL, OP_DEL, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f},
        },
    {
        {OP_DEL, OP_DEL, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f},
        {OP_DEL, OP_DEL, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f},
        {OP_DEL, OP_DEL, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0xbf},
        },
    {
        {OP_DEL, OP_DEL, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f},
        {OP_DEL},
        {OP_SWAP, OP_DEL},
        },
    {
        {OP_DEL, OP_DEL, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f},
        {OP_SWAP, OP_DEL, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f, OP_SWAP, OP_DIV},
        {OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, OP_POW, OP_DIV, OP_NEG},
        },
    {
        {OP_DUP, OP_PICK, 2, OP_MUL, OP_SWAP, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f, OP_SUB, OP_POW, OP_SWAP, OP_DEL},
        {OP_SWAP, OP_DUP, OP_LN, OP_SWAP, OP_PICK, 2, OP_POW, OP_MUL, OP_SWAP, OP_DEL},
        },
    {
        {OP_DEL, OP_PUSH, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f, OP_DONE},
        {OP_EXP, OP_DONE},
        {OP_DONE},
        },
    {
        {OP_LN, OP_MUL, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f, OP_SWAP, OP_DIV},
        {OP_DUP, OP_LN, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, OP_POW, OP_MUL, OP_SWAP, OP_LN, OP_SWAP, OP_DIV},
        },
    {
        {OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f, OP_SWAP, OP_DIV},
        {},
        },
    {
        {OP_DEL, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f},
        {},
        },
    {
        {OP_DUP, OP_ABS, OP_DIV},
        {},
        },
    {
        {OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, OP_MUL, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f, OP_SWAP, OP_DIV},
        {},
        },
    {
        {OP_DUP, OP_DUP, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f, OP_SWAP, OP_SUB, OP_SWAP, OP_DIV, OP_PICK, 2, OP_SWAP, OP_POW, OP_SWAP, OP_DIV, OP_SWAP, OP_DEL},
        {OP_DUP, OP_LN, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3f, OP_PICK, 2, OP_DIV, OP_POW, OP_SWAP, OP_PUSH, CH_CONSTANT, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, OP_POW, OP_DIV},
        },
    {
        {OP_COS},
        {},
        },
    {
        {OP_SIN},
        {},
        },
};

typedef struct Instruction Instruction;

typedef union {
    byte* basic;
    Instruction* complex;
} InstructionArgument;

typedef enum {
    COM_BASIC,
    COM_COMPLEX,
} InstructionComplexity;

struct Instruction{
    int param_count;
    double* params;

    int variable;

    InstructionArgument args[2];
    InstructionComplexity complexity[2];

    byte* op;
};

typedef struct {
    ChangeType change;

    double value;
} Value;

struct {
    byte* ip;

    Value stack[256];
    Value* stack_ptr;

    double result;
} VM;

typedef enum {
    COD_SUCCESS,
    COD_DIV_BY_ZERO,
    COD_STACK_OVERFLOW,
    COD_STACK_UNDERFLOW,
    COD_INVALID_INSTRUCTION,
} ExecuteCode;

void push(ChangeType change, double value) {
    VM.stack_ptr->value = value;
    VM.stack_ptr++->change = change;
}

Value pop() {
    return *--VM.stack_ptr;
}

void resetVM() {
    VM = (typeof(VM)) {};

    VM.stack_ptr = VM.stack;
    VM.result = 0;
}

ExecuteCode execute(byte* code) {
    VM.ip = code;

    Value temp1;
    Value temp2;

    for (;;) {
        switch (*VM.ip++) {
            case OP_PUSH:
                push(*VM.ip, *(double*)(VM.ip + 1));

                VM.ip += 9;
                break;
            case OP_POP:
                VM.result = pop().value;
                break;
            case OP_DUP:
                temp1 = pop();
                
                push(temp1.change, temp1.value);
                push(temp1.change, temp1.value);
                break;
            case OP_DEL:
                pop();
                break;
            case OP_PICK:
                temp1 = *(VM.stack_ptr - *VM.ip++ - 1);
                push(temp1.change, temp1.value);

                break;
            case OP_SWAP:
                temp1 = pop();
                temp2 = pop();

                push(temp2.change, temp2.value);
                push(temp1.change, temp1.value);
                break;
            case OP_DONE:
                return COD_SUCCESS;
            case OP_ADD:
                temp1 = pop();
                temp2 = pop();

                push(temp1.change | temp2.change, temp1.value + temp2.value);
                break;
            case OP_SUB:
                temp1 = pop();
                temp2 = pop();

                push(temp1.change | temp2.change, temp1.value - temp2.value);
                break;
            case OP_MUL:
                temp1 = pop();
                temp2 = pop();

                push(temp1.change | temp2.change, temp1.value * temp2.value);
                break;
            case OP_DIV:
                temp1 = pop();
                temp2 = pop();

                if (temp2.value == 0) return COD_DIV_BY_ZERO;

                push(temp1.change | temp2.change, temp1.value * temp2.value);
                break;
            case OP_POW:
                temp1 = pop();
                temp2 = pop();

                push(temp1.change | temp2.change, pow(temp1.value, temp2.value));
                break;
            case OP_EXP:
                temp1 = pop();

                push(temp1.change, exp(temp1.value));
                break;
            case OP_LOG:
                temp1 = pop();
                temp2 = pop();

                push(temp1.change | temp2.change, log(temp1.value) / log(temp2.value));
                break;
            case OP_LN:
                temp1 = pop();

                push(temp1.change, log(temp1.value));
                break;
            case OP_SQRT:
                temp1 = pop();

                push(temp1.change, sqrt(temp1.value));
                break;
            case OP_NQRT:
                temp1 = pop();
                temp2 = pop();

                push(temp1.change | temp2.change, pow(temp1.value, 1.0 / temp2.value));
                break;
            case OP_SIN:
                temp1 = pop();

                push(temp1.change, sin(temp1.value));
                break;
            case OP_COS:
                temp1 = pop();

                push(temp1.change, cos(temp1.value));
                break;
            default:
                return COD_INVALID_INSTRUCTION;
        }
    }
}

void pushParams(Instruction* instruction) {
    byte push_code[11] = {OP_PUSH, 0, 0, 0, 0, 0, 0, 0, 0, 0, OP_DONE};

    for (int i = 0; i < instruction->param_count; i++) {
        push_code[1] = i == instruction->variable;
        memcpy(push_code + 2, instruction->params + i, 8);

        execute(push_code);
    }
}

ExecuteCode executeInstruction(Instruction* instruction) {
    ExecuteCode code;

    for (int i = 0; i < 2; i++) {
        pushParams(instruction);

        if (instruction->complexity[i] == COM_BASIC) {
            code = execute(instruction->args[i].basic);
            if (code != COD_SUCCESS) return code;
        }
        else {
            code = executeInstruction(instruction->args[i].complex);
            if (code != COD_SUCCESS) return code;
        }

        VM.stack_ptr = VM.stack + i + 1;
    }

    execute(instruction->op);

    return COD_SUCCESS;
}

int codeLength(byte* code) {
    int i = 0;
    for (i = 0; code[i] != OP_DONE; i++);

    return i;
}

byte* getDerivative(byte* code) {
    int i;
    for (i = 0; code[i] < 7; i++);

    byte op = code[i];

    code[i] = OP_DONE;
    execute(code);
    code[i] = op;

    int variable = VM.stack_ptr[-1].change ? 1 : VM.stack_ptr[-2].change ? 2 : 0;

    sder:
        byte* single_derivative = derivatives[code[i] - 7][variable];
        int single_derivative_length = codeLength(single_derivative) - 1;

    if (!single_derivative_length) {
        variable = 0;
        goto sder;
    }

    byte* derivative = malloc(codeLength(code) + single_derivative_length);

    memcpy(derivative, code, i);
    memcpy(derivative + i, single_derivative, single_derivative_length);
    memcpy(derivative + i + single_derivative_length, code + i + 1, codeLength(code) - i - 1);

    VM.stack_ptr = VM.stack;

    return derivative;
}

int main(void) {
    resetVM();

    byte prepare[] = {OP_PUSH, CH_VARIABLE, 0x5c, 0x8f, 0xc2, 0xf5, 0x28, 0x5c, 0x1e, 0x40, OP_PUSH, CH_CONSTANT, 0x1f, 0x85, 0xeb, 0x51, 0xb8, 0x1e, 0x9, 0x40, OP_DONE};

    byte code[] = {OP_DEL, OP_EXP, OP_POP, OP_DONE};

    execute(prepare);

    byte* derivative = getDerivative(code);

    execute(derivative);

    printf("%f\n", VM.result);

    return 0;
}

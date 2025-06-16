#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>

// Tamanho máximo de uma linha no arquivo .asm
#define MAX_LINE_LENGTH 256
// Tamanho de uma instrução RISC-V em bits
#define INSTRUCTION_LENGTH_BITS 32

// Função principal de montagem
void assemble_file(FILE *input_file, FILE *output_file);

#endif // ASSEMBLER_H
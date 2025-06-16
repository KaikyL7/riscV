#include "assembler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h> // Adicionado para fprintf e snprintf

// Função auxiliar para converter um registrador (ex: "x10") para seu número inteiro
int parse_register(const char *reg_str) {
    if (reg_str[0] == 'x') {
        return atoi(reg_str + 1);
    }
    return -1; // Erro
}

// Função auxiliar para converter um número inteiro para sua representação binária de n bits
void to_binary_string(unsigned int value, int num_bits, char *buffer) {
    for (int i = num_bits - 1; i >= 0; i--) {
        buffer[num_bits - 1 - i] = ((value >> i) & 1) ? '1' : '0';
    }
    buffer[num_bits] = '\0';
}

void assemble_file(FILE *input_file, FILE *output_file) {
    char line[MAX_LINE_LENGTH];
    char binary_instruction[INSTRUCTION_LENGTH_BITS + 1];

    while (fgets(line, sizeof(line), input_file) != NULL) {
        // Remover quebras de linha
        line[strcspn(line, "\n")] = 0;

        // Ignorar linhas vazias ou comentários
        if (strlen(line) == 0 || line[0] == '#') {
            continue;
        }

        char *token = strtok(line, " ,\t");
        if (token == NULL) {
            continue;
        }

        unsigned int instruction_code = 0;

        // lw rd, offset(rs1) - Tipo I
        // lw x1, 0(x2)
        if (strcmp(token, "lw") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *offset_str = strtok(NULL, "(");
            char *rs1_str = strtok(NULL, ")");

            int rd = parse_register(rd_str);
            int offset = atoi(offset_str);
            int rs1 = parse_register(rs1_str);

            // opcode: 0000011 (7 bits)
            // funct3: 010 (3 bits)
            instruction_code |= (unsigned int)offset << 20; // imm[11:0]
            instruction_code |= (unsigned int)rs1 << 15;    // rs1
            instruction_code |= (unsigned int)0b010 << 12; // funct3
            instruction_code |= (unsigned int)rd << 7;     // rd
            instruction_code |= (unsigned int)0b0000011;   // opcode
        }
        // sw rs2, offset(rs1) - Tipo S
        // sw x1, 0(x2)
        else if (strcmp(token, "sw") == 0) {
            char *rs2_str = strtok(NULL, " ,\t");
            char *offset_str = strtok(NULL, "(");
            char *rs1_str = strtok(NULL, ")");

            int rs2 = parse_register(rs2_str);
            int offset = atoi(offset_str);
            int rs1 = parse_register(rs1_str);

            // opcode: 0100011 (7 bits)
            // funct3: 010 (3 bits)
            unsigned int imm_11_5 = (unsigned int)(offset >> 5) & 0x7F; // imm[11:5]
            unsigned int imm_4_0 = (unsigned int)offset & 0x1F;        // imm[4:0]

            instruction_code |= imm_11_5 << 25;       // imm[11:5]
            instruction_code |= (unsigned int)rs2 << 20;     // rs2
            instruction_code |= (unsigned int)rs1 << 15;     // rs1
            instruction_code |= (unsigned int)0b010 << 12;  // funct3
            instruction_code |= imm_4_0 << 7;         // imm[4:0]
            instruction_code |= (unsigned int)0b0100011; // opcode
        }
        // add rd, rs1, rs2 - Tipo R
        // add x1, x2, x3
        else if (strcmp(token, "add") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *rs1_str = strtok(NULL, " ,\t");
            char *rs2_str = strtok(NULL, " ,\t");

            int rd = parse_register(rd_str);
            int rs1 = parse_register(rs1_str);
            int rs2 = parse_register(rs2_str);

            // opcode: 0110011 (7 bits)
            // funct3: 000 (3 bits)
            // funct7: 0000000 (7 bits)
            instruction_code |= (unsigned int)0b0000000 << 25; // funct7
            instruction_code |= (unsigned int)rs2 << 20;     // rs2
            instruction_code |= (unsigned int)rs1 << 15;     // rs1
            instruction_code |= (unsigned int)0b000 << 12;  // funct3
            instruction_code |= (unsigned int)rd << 7;       // rd
            instruction_code |= (unsigned int)0b0110011;   // opcode
        }
        // xor rd, rs1, rs2 - Tipo R
        // xor x1, x2, x3
        else if (strcmp(token, "xor") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *rs1_str = strtok(NULL, " ,\t");
            char *rs2_str = strtok(NULL, " ,\t");

            int rd = parse_register(rd_str);
            int rs1 = parse_register(rs1_str);
            int rs2 = parse_register(rs2_str);

            // opcode: 0110011 (7 bits)
            // funct3: 100 (3 bits)
            // funct7: 0000000 (7 bits)
            instruction_code |= (unsigned int)0b0000000 << 25; // funct7
            instruction_code |= (unsigned int)rs2 << 20;     // rs2
            instruction_code |= (unsigned int)rs1 << 15;     // rs1
            instruction_code |= (unsigned int)0b100 << 12;  // funct3
            instruction_code |= (unsigned int)rd << 7;       // rd
            instruction_code |= (unsigned int)0b0110011;   // opcode
        }
        // addi rd, rs1, imm - Tipo I
        // addi x1, x2, 100
        else if (strcmp(token, "addi") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *rs1_str = strtok(NULL, " ,\t");
            char *imm_str = strtok(NULL, " ,\t");

            int rd = parse_register(rd_str);
            int rs1 = parse_register(rs1_str);
            int imm = atoi(imm_str);

            // opcode: 0010011 (7 bits)
            // funct3: 000 (3 bits)
            instruction_code |= (unsigned int)(imm & 0xFFF) << 20; // imm[11:0] (sinal extendido)
            instruction_code |= (unsigned int)rs1 << 15;          // rs1
            instruction_code |= (unsigned int)0b000 << 12;       // funct3
            instruction_code |= (unsigned int)rd << 7;            // rd
            instruction_code |= (unsigned int)0b0010011;         // opcode
        }
        // sll rd, rs1, rs2 - Tipo R
        // sll x1, x2, x3 (shamt em sll é rs2)
        // Note: sll instrução Tipo R, slli instrução Tipo I.
        // Considerando o exemplo, estamos lidando com sll Tipo R onde rs2 é o shamt
        else if (strcmp(token, "sll") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *rs1_str = strtok(NULL, " ,\t");
            char *rs2_str = strtok(NULL, " ,\t");

            int rd = parse_register(rd_str);
            int rs1 = parse_register(rs1_str);
            int rs2 = parse_register(rs2_str); // Para sll, rs2 é o shift amount

            // opcode: 0110011 (7 bits)
            // funct3: 001 (3 bits)
            // funct7: 0000000 (7 bits)
            instruction_code |= (unsigned int)0b0000000 << 25; // funct7
            instruction_code |= (unsigned int)rs2 << 20;     // rs2 (shamt para sll)
            instruction_code |= (unsigned int)rs1 << 15;     // rs1
            instruction_code |= (unsigned int)0b001 << 12;  // funct3
            instruction_code |= (unsigned int)rd << 7;       // rd
            instruction_code |= (unsigned int)0b0110011;   // opcode
        }
        // bne rs1, rs2, label - Tipo B (com offset para label)
        // Para simplificação, vamos assumir que o offset será fornecido diretamente como um imediato.
        // Se a implementação exigisse labels, uma segunda passagem seria necessária.
        // bne x1, x2, offset_direto (offset é o imediato)
        else if (strcmp(token, "bne") == 0) {
            char *rs1_str = strtok(NULL, " ,\t");
            char *rs2_str = strtok(NULL, " ,\t");
            char *offset_str = strtok(NULL, " ,\t");

            int rs1 = parse_register(rs1_str);
            int rs2 = parse_register(rs2_str);
            int offset = atoi(offset_str); // offset em bytes, dividido por 2 para palavras de 16 bits, mas aqui o offset é o imediato.

            // opcode: 1100011 (7 bits)
            // funct3: 001 (3 bits)
            // Imediato de 12 bits (imm[12|10:5|4:1|11])
            unsigned int imm_12 = (unsigned int)((offset >> 11) & 0x1);
            unsigned int imm_10_5 = (unsigned int)((offset >> 5) & 0x3F);
            unsigned int imm_4_1 = (unsigned int)((offset >> 1) & 0xF);
            unsigned int imm_11 = (unsigned int)((offset >> 10) & 0x1); // imm[11]

            instruction_code |= imm_12 << 31;         // imm[12]
            instruction_code |= imm_10_5 << 25;       // imm[10:5]
            instruction_code |= (unsigned int)rs2 << 20;     // rs2
            instruction_code |= (unsigned int)rs1 << 15;     // rs1
            instruction_code |= (unsigned int)0b001 << 12;  // funct3
            instruction_code |= imm_4_1 << 8;         // imm[4:1]
            instruction_code |= imm_11 << 7;          // imm[11]
            instruction_code |= (unsigned int)0b1100011; // opcode
        }
        else {
            fprintf(stderr, "Erro: Instrução desconhecida ou não suportada: %s\n", token);
            continue;
        }

        to_binary_string(instruction_code, INSTRUCTION_LENGTH_BITS, binary_instruction);
        fprintf(output_file, "%s\n", binary_instruction);
    }
}
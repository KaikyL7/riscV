# Montador RISC-V

Este repositório contém uma implementação em C de um montador RISC-V simplificado. O projeto visa converter código Assembly RISC-V para seu equivalente binário, focado em um subconjunto específico de instruções do conjunto de instruções RISC-V.

Este montador foi desenvolvido como parte de um estudo aprofundado da arquitetura RISC-V e do processo de montagem. Ele lida com a tradução de instruções Assembly textuais para o formato binário de 32 bits, que pode ser interpretado por simuladores ou hardwares RISC-V compatíveis.

Atualmente, o montador suporta as seguintes instruções do ISA RISC-V:

lw (Load Word)
sw (Store Word)
add (Add)
xor (Exclusive OR)
addi (Add Immediate)
sll (Shift Left Logical)
bne (Branch if Not Equal)


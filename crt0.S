// Replaces the selected function's jr ra; its original delay slot stays intact.
.set noreorder
.section .text.entry,"ax",@progbits
.align 4
.globl _start
.type _start,@function
.ent _start
_start:
    addiu $sp,$sp,-0x60
    sq $ra,0x20($sp)
    sq $v0,0x30($sp)
    sq $v1,0x40($sp)
    sq $gp,0x50($sp)
    jal main
    nop
    lq $gp,0x50($sp)
    lq $v1,0x40($sp)
    lq $v0,0x30($sp)
    lq $ra,0x20($sp)
    jr $ra
    addiu $sp,$sp,0x60
.end _start
.size _start,.-_start

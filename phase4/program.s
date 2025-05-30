# Assembly code generated by Roadmap Compiler Phase 4
# Target Architecture: x86_64
# Source: Logical expressions (B = TRUE, C = FALSE, B OR C)
#

.section .text
.global _start

_start:
    # Program entry point
    # Generated code begins
    movq     $1, %rax    # Load B = TRUE
    movq     $0, %rbx    # Load C = FALSE
    orq      %rbx, %rax    # B OR C (result in RAX)
    movq     $0, %rax    # Set exit code to 0

    # System exit
    mov $60, %rax    # sys_exit system call
    mov $0, %rdi     # exit status (success)
    syscall           # invoke system call

# End of generated assembly

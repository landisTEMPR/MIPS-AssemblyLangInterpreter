# Test program 3: Loop demonstration
# Author: Brysen Landis
# Counts from 1 to 10

.data
    msg: .asciiz "Count: "
    newline: .asciiz "\n"

.text
.globl main

main:
    li $t0, 1          # counter = 1
    li $t1, 10         # max = 10

loop:
    # Print message
    li $v0, 4
    la $a0, msg
    syscall
    
    # Print counter
    li $v0, 1
    move $a0, $t0
    syscall
    
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
    
    # Increment counter
    addi $t0, $t0, 1
    
    # Check if done
    blt $t0, $t1, loop
    beq $t0, $t1, loop  # Also print 10
    
    # Exit
    li $v0, 10
    syscall

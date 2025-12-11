# Test program 1: Basic arithmetic and syscalls
# Author: Brysen Landis

.data
    msg1: .asciiz "Enter first number: "
    msg2: .asciiz "Enter second number: "
    result_msg: .asciiz "Sum is: "
    newline: .asciiz "\n"

.text
.globl main

main:
    # Print first prompt
    li $v0, 4
    la $a0, msg1
    syscall
    
    # Read first integer
    li $v0, 5
    syscall
    move $t0, $v0
    
    # Print second prompt
    li $v0, 4
    la $a0, msg2
    syscall
    
    # Read second integer
    li $v0, 5
    syscall
    move $t1, $v0
    
    # Add numbers
    add $t2, $t0, $t1
    
    # Print result message
    li $v0, 4
    la $a0, result_msg
    syscall
    
    # Print sum
    li $v0, 1
    move $a0, $t2
    syscall
    
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
    
    # Exit
    li $v0, 10
    syscall

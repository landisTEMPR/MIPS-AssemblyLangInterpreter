# Test program 2: Factorial calculation
# Author: Brysen Landis
# Calculates factorial of a number using recursion

.data
    prompt: .asciiz "Enter a number: "
    result_msg: .asciiz "Factorial is: "
    newline: .asciiz "\n"

.text
.globl main

main:
    # Print prompt
    li $v0, 4
    la $a0, prompt
    syscall
    
    # Read integer
    li $v0, 5
    syscall
    move $a0, $v0
    
    # Call factorial
    jal factorial
    move $t0, $v0
    
    # Print result message
    li $v0, 4
    la $a0, result_msg
    syscall
    
    # Print factorial
    li $v0, 1
    move $a0, $t0
    syscall
    
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
    
    # Exit
    li $v0, 10
    syscall

factorial:
    # Base case: if n <= 1, return 1
    slti $t0, $a0, 2
    beq $t0, $zero, recursive
    li $v0, 1
    jr $ra

recursive:
    # Save return address and argument
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $a0, 0($sp)
    
    # Calculate factorial(n-1)
    addi $a0, $a0, -1
    jal factorial
    
    # Restore argument
    lw $a0, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    
    # Multiply n * factorial(n-1)
    mult $a0, $v0
    mflo $v0
    
    jr $ra

# Advanced Test: Array Sum and Average
# Author: Brysen Landis
# Demonstrates array manipulation, loops, and function calls

.data
    array: .word 10, 25, 3, 42, 17, 8, 33, 19, 5, 14
    size: .word 10
    sum_msg: .asciiz "Sum: "
    avg_msg: .asciiz "\nAverage: "
    newline: .asciiz "\n"

.text
.globl main

main:
    # Load array address and size
    la $a0, array
    lw $a1, size
    
    # Calculate sum
    jal array_sum
    move $s0, $v0      # Save sum
    
    # Print sum
    li $v0, 4
    la $a0, sum_msg
    syscall
    
    li $v0, 1
    move $a0, $s0
    syscall
    
    # Calculate average
    lw $a1, size
    move $a0, $s0
    jal divide
    move $s1, $v0      # Save quotient
    
    # Print average
    li $v0, 4
    la $a0, avg_msg
    syscall
    
    li $v0, 1
    move $a0, $s1
    syscall
    
    li $v0, 4
    la $a0, newline
    syscall
    
    # Exit
    li $v0, 10
    syscall

# Function: array_sum
# Arguments: $a0 = array address, $a1 = size
# Returns: $v0 = sum
array_sum:
    li $t0, 0          # sum = 0
    li $t1, 0          # i = 0
    move $t2, $a0      # array pointer
    
sum_loop:
    beq $t1, $a1, sum_done   # if i == size, done
    
    lw $t3, 0($t2)     # load array[i]
    add $t0, $t0, $t3  # sum += array[i]
    
    addi $t2, $t2, 4   # advance pointer
    addi $t1, $t1, 1   # i++
    j sum_loop

sum_done:
    move $v0, $t0      # return sum
    jr $ra

# Function: divide
# Arguments: $a0 = dividend, $a1 = divisor
# Returns: $v0 = quotient
divide:
    div $a0, $a1
    mflo $v0
    jr $ra

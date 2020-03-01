
.data
buffer:      .space  1024
input_str:   .asciiz "Input string: "   
input_char:  .asciiz "Input character: "
out_str:     .asciiz "\nOutput: "

.text
.globl main

main:
    #pirnt string
	la $a0, input_str
	li $v0, 4
	syscall
	
	#read string
	la $a0, buffer
	li $a1, 1024
	li $v0, 8 
	syscall
	#print string
	la $a0, input_char
	li $v0, 4
	syscall
	
	#read character
	li $v0, 12
	syscall
	move $t3, $v0
	
	#print \n
	li $a0, '\n'
	li $v0, 11
	syscall
	
	#print string
	la $a0, input_char
	li $v0, 4
	syscall
	
	#read character
	li $v0, 12
	syscall
	move $t4, $v0
	
	#$t3 - old char
	#$t4 - new char
	
	la $t1, buffer
input_loop:
	lb $t0, 0($t1)
	beq $t0, $0, input_loop_exit
	
	bne $t0, $t3, next
	sb $t4, 0($t1)
next:	
	addi $t1, $t1, 1
	j input_loop
input_loop_exit:
		
	#print string
	la $a0, out_str
	li $v0, 4
	syscall	
	#print string
	la $a0, buffer
	li $v0, 4
	syscall
	
    li $v0, 10 
    syscall # Exit
	

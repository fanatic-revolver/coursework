	.data
as1:	.asciiz "please enter number n.\n"
nl:	.asciiz "\n"
sp:	.asciiz " "			
err1:	.asciiz "You need to enter a positive integer. try again.\n"	#declare the strings that may be used in the program	
buf:	.space	13		#input buffer		
	.text
	.globl main
main:	jal GetNum

	addi $s0,$v0,2		#let $s0(total rows + 2) equals to the number entered plus 2(to make sure the program to stop at a proper case)
	addi $s1,$zero,1	#set $s1(column) to 1
	j loop1			#start the first layer of loop

loop1:	addi $s1,$s1,1		#increase the number of rows by 1
	slt $t0,$s1,$s0		
	beq $t0,$zero,Exit	#if present row + 1 >= total rows + 2(that is, present row > total row),exit

	la $a0,nl	
	li $v0,4
	syscall			#print "\n" to start a new line

	addi $s2,$zero,1	#reinitialize $s2(column)
	j loop2			#start the second layer of loop

loop2:	slt $t1,$s2,$s1		#judge if the column >= row + 1(that is, column > row)
	beq $t1,$zero,loop1	#if column >= row + 1,go back to loop1
	move $a0,$s2

	li $v0,1
	syscall			#print the number equals to the present column

	la $a0,sp
	li $v0,4
	syscall			#print a white space to seperate each two numbers

	addi $s2,$s2,1		#increase $s2(column) by 1
	j loop2			#reloop2

err:  	la $a0,err1
	li $v0,4
	syscall			#print the error message 
	
	j main			#then back to the beginning of the program

Exit:	li $v0,10		#exit
        syscall
        
#----procedure: get a non-negative integer-----------------------------------------------------        
GetNum:
	li $s6,0xa		#$s6 = 10, it's the ascii of '\n'
	la $a0,as1
	li $v0,4
	syscall			
	la $a0,buf
	la $a1,12		#restrict the length of input, as a integer is at most 2147483648(ten digits)
	li $v0,8
	syscall			#read the input
	
	lb $t0,($a0)
	beq $t0,$s6,err		#if the users didn't enter anything but a '\n', goto err(error message)
	
	addi $sp,$sp,-4		
	sw $ra,($sp)		#this $ra is the address back to the main
	jal test		#goto sub-procedure:test
	lw $ra,($sp)
	addi $sp,$sp,4
	jr $ra			#return to main


test:	la $t0,buf		
	li $t7,0		
	li $v0,0
	li $t5,1 		#$t7 = the location of the present character, $v0 = the result, $t5 = the power of 10


gloop:	lb $s0,($t0)
	beq $s0,$s6,strend	#$s6 = 10('\n') means it's the end of the input,goto strend 
	
	li $s7,0x30		#the code here checks if the present character is between '0' and '9'
	sub $t6,$s0,$s7
	slt $t6,$t6,$zero
	bne $t6,$zero,err
	li $s7,0x39
	sub $t6,$s7,$s0
	slt $t6,$t6,$zero
	bne $t6,$zero,err	#the code here check if the present character is between '0' and '9'

	addi $t7,$t7,1		#count ++
	addi $t0,$t0,1		#the pointer to buffer move to the next character
	j gloop

strend:	addi $t0,$t0,-1		#the pointer to buffer move back to the last character
	move $t1,$t7		#$t1 = the length of the input

gloop1:	
	lb $s0,($t0)
	addi $s0,$s0,-0x30	#convert the character to integer
	beq $t7,1,judofl	#if the present character is the last one, goto judofl
	
iter:	mul $s0,$s0,$t5		
	add $v0,$v0,$s0
	beq $t7,1,return	#when it's the first character, return the result	
	addi $t7,$t7,-1		#location - 1
	addi $t0,$t0,-1		#the pointer to buffer move to last character
	li $t6,10		#here use $t6 the same as in jud
	mul $t5,$t5,$t6
	j gloop1

judofl:	li $s1,10
	sub $s1,$t1,$s1
	slt $s1,$s1,$zero
	bne $s1,$zero,noofl	#if then input is less than 10 digits, there is no overflow, goto noofl
	li $s1,3
	sub $s1,$s0,$s1
	slt $s1,$s1,$zero	
	beq $s1,$zero,err	#if $s0 >= 3 then overflow
	lui $s1,0x7fff
	ori $s1,0xffff		#$s1 is the largest positive integer
	mul $s2,$s0,$t5		#$s2 is the value biggest digit
	sub $s1,$s1,$s2		#$s2 is the value of rest of digits
	sub $s1,$s1,$v0
	slt $s1,$s1,$zero	
	beq $s1,$zero,iter	#if the final result < 2147483648, back to iter
	j err

noofl:	j iter

return:	jr $ra	
#--------------------------------------------------------------------------------------------------            
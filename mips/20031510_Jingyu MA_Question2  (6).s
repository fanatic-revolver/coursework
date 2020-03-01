	.data
numx:	.asciiz "please enter number x: "
numy:	.asciiz "please enter number y: "
err1:	.asciiz "You need to enter a positive integer(< 2147483648). try again.\n"
of:	.asciiz "Overflow!\n"			#declare the messages may be used in the program
buf:	.space	14				#input buffer	
	.text
	.globl main
main:	
	la $a0,numx
	li $v0,4
	syscall			#tell user to enter number x

	jal GetNum		#get a integer
	move $s0,$v0		#and store in $s1
	
	la $a0,numy
	li $v0,4
	syscall			#tell user to enter number y

	addi $sp,$sp,-4
	sw $s0,($sp)		#push $s0 into stack
	jal GetNum		#get a integer
	move $s1,$v0		#and store in $s1
	lw $s0,($sp)		#pop the first value in stack to $s0 
	addi $sp,$sp,4

	move $a0,$s0
	move $a1,$s1
	jal Add			#goto add x and y and check overflow
	move $s2,$v0		#store x + y into $s2

	move $a0,$s1
	move $a1,$s1
	jal Add			#goto add y and y and check overflow		
	move $s3,$v0		#store 2y into $s3				
	
	mult $s2,$s2		#(x + y) * (x + y)
	mflo $a0		#store the last 32bit of result in $a0
	mfhi $a1		#store the first 32bit of result in $a1
	jal Mulofl		#check overflow			
	
	mflo $s4		#store the result in $s4	

	mult $s4,$s2		#$s4 * (x + y)
	mflo $a0		#store the last 32bit of result in $a0
	mfhi $a1		#store the first 32bit of result in $a1
	jal Mulofl		#check overflow	

	mflo $s5		#store the result in $s5		

	#Calculation of (2y)3
	mult $s3,$s3		#(2y) * (2y)
	mflo $a0		#store the last 32bit of result in $a0
	mfhi $a1		#store the first 32bit of result in $a1
	jal Mulofl		#check overflow	
	
	mflo $s6		#store the result in $s5		

	mult $s6,$s3		#(2y) * (2y) * (2y)
	mflo $a0		#store the last 32bit of result in $a0
	mfhi $a1		#store the first 32bit of result in $a1
	jal Mulofl		#check overflow	
	
	mflo $s7		#store the result in $s6
	
	move $a0,$s5
	move $a1,$s7
	jal Add			#goto add (x+y)3 and 8y3 and check overflow
	move $s2,$v0		#store the result into $s2

	j prt


#add $a0 and $a1 and return the result as $v0 if no overflow
#if there are overflow, goto Ofl
Add:	slt $t0,$a0,$zero	#check if $a0 < 0
	bne $t0,$zero,ne	#if $a0 < 0, goto ne(negative $a0)

	slt $t0,$a1,$zero	#check if $a1 < 0
	beq $t0,$zero,pp	#if $a1 >= 0, goto pp(both $a0 and $a1 are non-negative)

Fadd:	#the final step of addition
	add $v0,$a1,$a0

	jr $ra			#return

ne:	#$a0 is negative
	slt $t0,$a1,$zero	#check if $a1 < 0
	beq $t0,$zero,Fadd	#if $a1 >= 0(here $a0 < 0, as a positive num plus a negative num can't overflow),
				#goto Fadd(add $a0 and $a1), else continue(both $a0 and $a1 are negative)

	lui $t0,0x8000		#$t0 = 0x80000000, the smallest 32bit negative integer
	sub $t0,$t0,$a0		#$t0 = $t0 - $a1	
	sub $t0,$t0,$a1		#$t0 = $t0 - $a1

	beq $t0,$zero,Fadd	#if $t0 = 0($a1 + $a1 = 0x80000000), goto Fadd(add $a0 and $a1)
	slt $t1,$t0,$zero
	beq $t1,$zero,Ofl	#if $t0 > 0($a1 + $a1 < 0x80000000), goto Ofl(Overflow)
	j Fadd			#else, goto Fadd(add $a0 and $a1) 
	
pp:	#both $a0 and $a1 are non-negative
	lui $t0,0x7fff
	ori $t0,0xffff		#$t0 = 0x7fffffff, the biggest 32bit positive integer
	sub $t0,$t0,$a0		#$t0 = $t0 - $a1	
	sub $t0,$t0,$a1		#$t0 = $t0 - $a1
	slt $t1,$t0,$zero
	bne $t1,$zero,Ofl	#if $t0 < 0($a1 + $a1 < 0x80000000), goto Ofl(Overflow)
	j Fadd			#else, goto Fadd(add $a0 and $a1) 
#addition and check overflow 


#check overflow in multiplication	
Mulofl:	slt $t0,$a0,$zero	#check if $a0 < 0
	bne $t0,$zero,mngtv	#if $a0 < 0, goto mngtv
	bne $a1,$zero,Ofl	#if Hi($a1) != 0, then goto Overflow

return:	jr $ra			#return

	
mngtv:	#Lo($a1) is negative
	lui $t0,0xffff
	ori $t0,0xffff		#$t0 = -1
	bne $a1,$t0,Ofl		#if Hi($a1) != -1, then goto Overflow

	j return
#check overflow in multiplication


Ofl:	la $a0,of 		#overflow
	li $v0,4
	syscall			#print overflow message
	j Exit			#goto exit


prt:	move $a0,$s2		
	li $v0,1
	syscall			#print out the outcome

	j Exit			#exit


Exit:   li $v0,10	
        syscall			#exit
        
        
#----procedure: get a integer-----------------------------------------------------        
GetNum:
	li $s6,45		#$s6 = 10, it's the ascii of '\n'		
	la $a0,buf
	la $a1,13		#restrict the length of input, as a integer is at most 2147483648(ten digits)
	li $v0,8
	syscall			#read the input
	
				
	lb $t0,($a0)	
	beq $t0,$s6,abs1
	
scheck:	li $s6,0xa		#checking if the second character is '\n'
	beq $t0,$s6,err		#if the users didn't enter anything but a '\n', goto err(error message)
	
	addi $sp,$sp,-4		
	sw $ra,($sp)		#this $ra is the address back to the main
	jal test		#goto sub-procedure:test
	lw $ra,($sp)
	addi $sp,$sp,4
	jr $ra			#return to main

abs1:	addi $a0,$a0,1		#move the pointer to next character
	li $s4,1		#set $s4 = 1 to mark that the number is a negative number
	j scheck

test:	move $t0,$a0		
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
	bne $t6,$zero,err	#the code here checks if the present character is between '0' and '9'

	addi $t7,$t7,1		#count ++
	addi $t0,$t0,1		#the pointer to buffer move to the next character
	j gloop

strend:	addi $t0,$t0,-1		#the pointer to buffer move back to the last character
	move $t1,$t7		#$t1 = the length of the input
	li $s6,11
	sub $s6,$t1,$s6
	slt $s6,$s6,$zero
	beq $s6,$zero,err

gloop1:	
	lb $s0,($t0)
	addi $s0,$s0,-0x30	#convert the character to integer, as (the ascii of a number character) - 48 = the number
	beq $t7,1,judofl	#if the present character is the last one, goto judofl
	
iter:	mul $s0,$s0,$t5		
	add $v0,$v0,$s0
	beq $t7,1,return1	#when it's the first character, return the result	
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

err:	la $a0,err1
	li $v0,4
	syscall
	j GetNum

return1:
	beq $s4,1,negt	
	jr $ra	

negt:	sub $v0,$zero,$v0
	li $s4,0
	j return1
#--------------------------------------------------------------------------------------------------    	
         
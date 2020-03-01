.data
num:  .float 0.000001
half: .float 0.5
msg:.asciiz"Please input a number:"
msg_error:.asciiz"An error has occured!"

#scanf("%d",&n); $f0:n:x0
#$f1:(0.5)*n:x1
.text
la $a0 msg    #print the message "Please input a number"
li $v0,4   
syscall
li $v0,6      #read a float number from the keyboard 
syscall 
mov.s $f0,$f0  #f0 what we read from the keyboard,namely,n
mov.s $f31,$f0 #f31=n
c.lt.s $f31,$f29 #if n<0 ,f29 is currently zero
bc1t error      # jump to error,because there can not compute a square root of negative number here

la $a0,half     #load the address of half which is storeing a floating number 0.50000 
l.s $f6,0($a0)  #let f6 become 0.5 using 0.5
mul.s $f1,$f0,$f6  #let f1 become 0.5 * n

sub.s $f2,$f0,$f1         #let f2 become n - 0.5 *n
abs.s $f2,$f2                            #f2=abs(n-0.5*n)



la $a0,num                #load 1ev6,which is 0.000001
l.d $f8, 0($a0)           #let f8=0.000001
selection:c.lt.s  $f8,$f2 #if 0.000001< abs(n-0.5*n) 
bc1t loop                 #jump to loop

mov.s $f12,$f1           #move result to f12
li $v0,2                 #print outcome
syscall
li $v0,10                #end program
syscall


loop:
mov.s $f0,$f1        #move 0.5*n to f0
div.s $f21,$f31,$f0  #f21=n/0.5*n
add.s $f22,$f21,$f0 #f22=0.5*n+n/0.5*n
mul.s $f1,$f6,$f22 #outcome f1=0.5*(0.5*n+n/0.5*n)
sub.s $f2,$f0,$f1  #f2=0.5*n-0.5*(0.5*n+n/0.5*n)
abs.s $f2,$f2      #take the absolute value of f2
j selection        #jump to selection

error:
la $a0 msg_error  #print error message
li $v0,4
syscall



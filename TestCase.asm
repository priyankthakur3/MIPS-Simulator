#Test cases for R type add,addu,sub, and
#               I addiu
#               J
.data


.text
addi $t1,$zero,5   #t1 = 5
add $t2,$t1,$t1    #t2 = 5 + 5 = 10	

addi $v0,$zero,1   
add $a0,$a0,$t2   #a0 = 10  print 10
syscall

addiu $t4,$zero,15
addiu $v0,$zero,1
add $a0,$zero,$t4  #a0 = 15,print 15
syscall

and $t3,$t1,$t2   #t3 = 5 & 10 =  0   
addi $v0,$zero,1                       
addi $a0, $zero,0
j next
addi $a0,$zero,35   #a0 = 35, should print 35, but we use j before ,so it wont print 35.
syscall


next:
and $t3,$t1,$t2   #t3 = 5 & 10 =  0   
addi $v0,$zero,1                       
addi $a0, $zero,0
add $a0,$a0,$t3   #a0 = 0, print 0
syscall


addi $v0,$zero,10
syscall

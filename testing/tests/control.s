    #test 2

    Li x1, 10
    Li x2, 20
    Li x3, 30
    blt x2, x1, L1
    li x25, 44


L1:
    li  x25, 10
    Bgt x3, x2, L2
    Li x4, -1  # error
L2:
    Li x4, 1
    Bgt x2, x3, Err
L3:
    Blt x2, x3, L4
    Li x5, -1 #error
    Nop
L4:
    li x5, 2
    j done

    Li x5, 1
    Blt x3, x2, Err
    Slt x6, x2, x1
    Slti x7, x3, 100
    Slt x8, x2, x3
    Slti x9, x3, 5
    Beq x3, x2, Err
    Bne x2, x2, Err
    J next

Second:
    Li x11, 20
    La x12, third
#    J Third
    Li x13, 50

Next:
    Li x10, 5
#    J second

Third:
    Jal x15, almost

#L20:
#    La x16, done
#Almost:
#    Li x20, 5
#    J L20
Err:
  Li x30, 60
  li a7, 10
  ecall
done:
  Li x31, 1
  li a7, 10
  ecall

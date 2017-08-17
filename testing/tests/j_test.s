    li  x4, 200
    jal x4, L1
    j   Err
    nop
    nop
L1:
    li  x1, 1
done:
    li x2, 2
    li a7, 10
    ecall

Err:
    li  x1, -1
    li a7, 10
    ecall

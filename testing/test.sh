#!/bin/sh
# change -O0 to -O2 for better compiler optimization
# O: optimization level fix

. ./env_setup.sh

name=$(echo $1 | cut -f 1 -d '.')
ext=$(echo $1 | cut -f 2 -d '.')

runSim (){  
    ${toolchain_path}riscv32-unknown-elf-objdump -d "$name.elf" > "$name.lst"
    ${toolchain_path}riscv32-unknown-elf-objcopy -O binary "$name.elf" "$name.bin"

    ../b2h.py "$name.bin" 1024 > "$name.hex" ## 
    #elf2hex 4 1024 "$name.elf" > "$name.hex"

    cp "$name.hex" "test.hex"

    ../rv32sim "$name.bin" | tail -32 > "$name.sim.txt"

    iverilog -Wall -Wno-timescale -o "$name.out" ../testbench.v ../../rtl/rv32.v ../../rtl/memory.v

    vvp "$name.out" | tail -32 > "$name.vvp.txt"




    diff -i -E  "$name.sim.txt" "$name.vvp.txt" > "$name.diff"

    if [ -s "$name.diff" ]
      then
            echo $name failed!
      else
            echo $name passed!
    fi

}

cd "$tmp_path"

if [ "$ext" = "s" ]
then
  ${toolchain_path}riscv32-unknown-elf-as -o "$name.elf" "$tests_path$1"
  runSim
else
    temp=$name
    for i in 0 1 2 3; do
        name=${temp}_O$i
        ${toolchain_path}riscv32-unknown-elf-gcc -Wall -O$i -march=rv32i -nostdlib -T ../link.ld -o "$name.elf" ../crt0_proj.S "$tests_path$1" -lgcc
        runSim
    done
fi

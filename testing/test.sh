#!/bin/sh
# change -O0 to -O2 for better compiler optimization
# O: optimization level fix

. ./env_setup.sh

name=$(echo $1 | cut -f 1 -d '.')
ext=$(echo $1 | cut -f 2 -d '.')

runSim (){
    ${toolchain_path}riscv32-unknown-elf-objdump -d -M no-aliases "$1.elf" > "$1.lst"
    ${toolchain_path}riscv32-unknown-elf-objcopy -O binary "$1.elf" "$1.bin"

    ../b2h.py "$1.bin" 1024 > "$1.hex" ## 
    #elf2hex 4 1024 "$1.elf" > "$1.hex"

    cp "$1.hex" "test.hex"

    ../rv32sim "$1.bin" | tail -32 > "$1.sim.txt"


    vvp "$name.out" | tail -32 > "$1.vvp.txt"




    diff -i -E  "$1.sim.txt" "$1.vvp.txt" > "$1.diff"

    if [ -s "$1.diff" ]
      then
            echo $1 failed!
      else
            echo $1 passed!
    fi

}

cd "$tmp_path"

if [ "$ext" = "s" ]
then
  ${toolchain_path}riscv32-unknown-elf-as -o "$name.elf" "$tests_path$1"
  iverilog -Wall -Wno-timescale -o "$name.out" ../testbench.v ../../rtl/rv32.v ../../rtl/memory.v
  runSim $name
else
    iverilog -Wall -Wno-timescale -o "$name.out" ../testbench.v ../../rtl/rv32.v ../../rtl/memory.v
    for i in 0 1 2 3; do
        ${toolchain_path}riscv32-unknown-elf-gcc -Wall -O$i -march=rv32i -nostdlib -T ../link.ld -o "${name}_O$i.elf" ../crt0_proj.S "$tests_path$1" -lgcc
        runSim ${name}_O$i
    done
fi

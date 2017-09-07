#! /bin/sh
# change -o0 to -o2 for better compiler optimization
. ./env_setup.sh

name=$(echo $1 | cut -f 1 -d '.')
ext=$(echo $1 | cut -f 2 -d '.')
run=${2:-0}                 #arg2 : optimization level

cd $tmp_path

if [ "$ext" = "s" ]
then
  ${toolchain_path}riscv32-unknown-elf-as -o "$name.elf" "$tests_path$1"
else
  ${toolchain_path}riscv32-unknown-elf-gcc  -Wall -O$run -march=rv32i -nostdlib -lgcc  -T ../link.ld -o "$name.elf" ../crt0_proj.S "$tests_path$1" -lgcc
fi

${toolchain_path}riscv32-unknown-elf-objdump -M no-aliases -d "$name.elf" > "$name.lst"
${toolchain_path}riscv32-unknown-elf-objcopy -O binary "$name.elf" "$name.bin"

../b2h.py "$name.bin" 1024 > "$name.hex"

cp "$name.hex" "test.hex"

#produce sim first
../rv32sim "$name.bin" | tail -32 > "$name.sim.txt"

iverilog   -Wall -Wno-timescale -o "$name.out" ../testbench.v ../../rtl/rv32.v ../../rtl/memory.v
vvp "$name.out" | tail -32 > "$name.vvp.txt"



diff -i -E  "$name.sim.txt" "$name.vvp.txt" > "$name.diff"

if [ -s "$name.diff" ]
  then
        echo $name failed!
  else
        echo $name \(with -O$run\) passed!
fi

#! /bin/sh
# change -o0 to -o2 for better compiler optimization
tests_path="../tests/"
name=$(echo $1 | cut -f 1 -d '.')
ext=$(echo $1 | cut -f 2 -d '.')

cd "tmp"

if [ "$ext" = "s" ]
then
  /Users/auc/work/CloudV/prv32i/rv32-gcc/bin/riscv32-unknown-elf-as -o "$name.elf" "$tests_path$1"
else
  /Users/auc/work/CloudV/prv32i/rv32-gcc/bin/riscv32-unknown-elf-gcc  -Wall -o4 -march=rv32i -nostdlib -lgcc  -T ../link.ld -o "$name.elf" ../crt0_proj.S "$tests_path$1" -lgcc
fi

/Users/auc/work/CloudV/prv32i/rv32-gcc/bin/riscv32-unknown-elf-objdump -d "$name.elf" > "$name.lst"
/Users/auc/work/CloudV/prv32i/rv32-gcc/bin/riscv32-unknown-elf-objcopy -O binary "$name.elf" "$name.bin"

../b2h.py "$name.bin" 1024 > "$name.hex"

cp "$name.hex" "test.hex"

iverilog   -Wall -Wno-timescale ../testbench.v ../../rtl/rv32.v ../../rtl/memory.v
vvp a.out | tail -32 > "$name.vvp.txt"

../rv32sim "$name.bin" | tail -32 > "$name.sim.txt"

diff -i -E  "$name.sim.txt" "$name.vvp.txt" > "$name.diff"

if [ -s "$name.diff" ]
  then
        echo $name failed!
  else
        echo $name passed!
fi

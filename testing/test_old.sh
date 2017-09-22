#! /bin/sh
# change -o0 to -o2 for better compiler optimization
. ./setup.sh

name=$(echo $1 | cut -f 1 -d '.')
ext=$(echo $1 | cut -f 2 -d '.')
run=${2:-0}                 #arg2 : optimization level

mode=${3:-0}                 #arg3 : 0: normal, 1: interrupts 2: special

if [ $mode = 1 ]; then
    tests_path="${tests_path}int/"
else
    if [ $mode = 2 ]; then
        tests_path="${tests_path}special/"
    fi
fi

cd $tmp_path

if [ "$ext" = "s" ]
then
  ${toolchain_path}riscv32-unknown-elf-as -o "$name.elf" "$tests_path$1"
else
  ${toolchain_path}riscv32-unknown-elf-gcc  -Wall -O$run -march=rv32i -nostdlib -lgcc  -T ../link.ld -o "$name.elf" ../crt0_proj.S "$tests_path$1" -lgcc
fi

${toolchain_path}riscv32-unknown-elf-objdump -M no-aliases -d "$name.elf" > "$name.lst"
${toolchain_path}riscv32-unknown-elf-objcopy -O binary "$name.elf" "$name.bin"

../b2h.py "$name.bin" "$CAPH" > "$name.hex"

cp "$name.hex" "test.hex"


iverilog   -Wall -Wno-timescale -o "$name.out" ../testbench.v ../../rtl/rv32.v ../../rtl/memory.v

../rv32sim "$name.bin" | tail -32 > "$name.sim.txt"
vvp -N "$name.out"  | tail -33 > "$name.vvp.txt"

PERF=$(head -n 1 "$name.vvp.txt")
sed -i '1 d' "$name.vvp.txt"

diff -i -E  "$name.sim.txt" "$name.vvp.txt" > "$name.diff"

if [ -s "$name.diff" ]
  then
        echo $name failed!
  else
	  echo $name \(with -O$run\) passed!\($PERF\)
fi

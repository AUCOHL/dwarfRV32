#!/bin/sh
# change -O0 to -O2 for better compiler optimization
# O: optimization level fix

. ./setup.sh

name=$(echo $1 | cut -f 1 -d '.')
ext=$(echo $1 | cut -f 2 -d '.')
subdir=$2
iver="iverilog -Wall -Wno-timescale -o "$name.out" ../testbench.v ../../rtl/rv32.v ../../rtl/memory.v ../../rtl/qspi/rom_qspi.v ../../rtl/regfile.v"

tests_path="$tests_path$2/"


runSim (){
    ${toolchain_path}riscv32-unknown-elf-objdump -d -M no-aliases "$1.elf" > "$1.lst"
    ${toolchain_path}riscv32-unknown-elf-objcopy -O binary "$1.elf" "$1.bin"
#	return

    ../b2h.py "$1.bin" "$CAPH" > "$1.hex" ## 

    cp "$1.hex" "test.hex"

    ../rv32sim "$1.bin" | tail -32 > "$1.sim.txt"
    vvp -N "$name.out"  | tail -33 > "$1.vvp.txt"

	PERF=$(head -n 1 "$1.vvp.txt")
	sed -i '1 d' "$1.vvp.txt"

	diff -i -E  "$1.sim.txt" "$1.vvp.txt" > "$1.diff"

    if [ -s "$1.diff" ]
      then
            echo $1 failed!
      else
		  echo $1 passed!\($PERF\)
    fi

}

cd "$tmp_path"

if [ "$ext" = "s" ]
then
    eval $iver
    ${toolchain_path}riscv32-unknown-elf-as "$tests_path$1" ../irq.S -o "$name.elf"
    runSim $name
else
    eval $iver
    for i in 0 1 2 3; do
        ${toolchain_path}riscv32-unknown-elf-gcc -Wall -O$i -march=rv32i -nostdlib -T ../link.ld ../crt0_proj.S "$tests_path$1" ../irq.S -o "${name}_O$i.elf" -lgcc
        runSim ${name}_O$i
    done
fi

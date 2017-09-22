MEMSIZE = 16384
NRUNS = 500

#OBJS = c0.o stdlibtest.o stdlib.o 
OBJS =  c0.o stdlib.o dhry_1.o dhry_2.o
CFLAGS = -MD -Wall -O3 -march=rv32i -nostdlib -DTIME -DRISCV -DNRUNS=$(NRUNS)
TOOLCHAIN_PREFIX = riscv32-unknown-elf-

test: test.out test.hex
		vvp -N test.out

test.out: testbench.v ../../rtl/rv32.v ../../rtl/memory.v
		iverilog -Wall -Wno-timescale -o test.out testbench.v ../../rtl/rv32.v ../../rtl/memory.v

test.hex: test.elf
		$(TOOLCHAIN_PREFIX)objcopy -O binary $< test.bin
		../b2h.py test.bin $(MEMSIZE) > $@

test.elf: $(OBJS) link.ld
		$(TOOLCHAIN_PREFIX)gcc $(CFLAGS) -T link.ld -o $@ $(OBJS) -lgcc
	 	$(TOOLCHAIN_PREFIX)objdump -d -M no-aliases $@ > test.lst


%.o: %.c 
		$(TOOLCHAIN_PREFIX)gcc -c $(CFLAGS) $<

c0.o:	c0.S
		$(TOOLCHAIN_PREFIX)gcc -c $(CFLAGS) $<

dhry_1.o dhry_2.o:
CFLAGS += -Wno-implicit-int -Wno-implicit-function-declaration

#stdlibtest.o:
	#	 CFLAGS += -Wno-implicit-int -Wno-implicit-function-declaration

clean:
		\rm *.o *.d test.elf test.bin test.out test.hex test.lst

.PHONY: test clean



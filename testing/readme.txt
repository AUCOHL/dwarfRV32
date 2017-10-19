1) To run a functional test: ./test.sh fact.c
    + fact.c is a functional test under the ./tests folder;
    + the temp files are placed under the ./tmp folder;
    + functional tests files are assembly (.s) and C (.c); the script recognize each using the extension
2) To run all the functional tests: ./testall.sh
    + tests all the test files (.c and .s) placed under ./tests folder
3) To run the random regression: ./stress.sh
    + these are 100 randomly generated tests;
    + temp files are placed under ./tmp/reg and generated sources under ./tests/reg
---
To change the memory size: (currently 16KB)
-LOGCAPH in memory.v should be at least [log2(CAPH)]; CAPH: the size of a memory bank
-adjust CAPH in setup.sh
-The memory in the tesbench should be instantiated with the correct CAPH parameter

====================================================================================
changelog:
==========
+ interrupts disabled for now; all tests (except int.s) pass without interrupts. (to be handled next)
+ env_setup.sh, and test.sh script tests all 4 optimization levels
+ rtl: shifter - alu control signals - "simulation terminator IR -> IR2"
+++
+ port names refactored to match the techreport (IRQen left!)
+++
+ rdtime/rdcycle mixed
+ time interrupts in an instruction following a taken branch/jump
+++
+ data dependency of rdtime/rdcycle fixed
+++
+ Dhrystone
+ CPI is displayed after each test case
+ Tested and fixed extensions (ext_mul)
+ Added basic support for extensions in rv32sim and randreg
+++
+ Implemented branch prediction //
+ Dhrystone revised (currently 0.73 DMIPS/MHz)
+ Integrated IntCtrl
+ Multivector interrupts (to be doc'ed; user provides IRi's; otherwise, default uret)
+ wfi (no constraints; additional logic for the hold)
+ fixed a bug resulting from a jump being followed by ecall/ebreak


====================================================================================
+ random+dijkstra fail; memory gets overwritten? (the produced code overwrites the text segment)


====================================================================================
notes:
======
+ b2h assertation error (%4)
+ $signed($signed(a) << sha) --> check again @ synthesis
+ optimize sys insts recog?
+ optimize ext/wfi hold ?
+ ChipSelect in rtl/memory.v (write/read)

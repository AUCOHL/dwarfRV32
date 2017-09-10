1) To run a functional test: ./test.sh fact.c
    + fact.c is a functional test under the ./tests folder;
    + the temp files are placed under the ./tmp folder;
    + functional tests files are assembly (.s) and C (.c); the script recognize each using the extension
2) To run all the functional tests: ./testall.sh
    + tests all the test files (.c and .s) placed under ./tests folder
3) To run the random regression: ./stress.sh
    + these are 100 randomly generated tests;
    + temp files are placed under ./tmp/reg and generated sources under ./tests/reg

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

====================================================================================
+ quicksort.c, mergesort.c, 6queens.c overflow the ram section with -O3/-Ofast only
+ random+dijkstra fail; memory gets overwritten? (the produced code overwrites the text segment)
+ gcc sometimes references memcpy and -mno-memcpy doesn't work (so, cannot initialize arrays larger than 7(?) inside main!) -> arrinmain.c:
    solution: maybe like ext_mul?
+ IntCtrl

====================================================================================
notes:
======
+ b2h assertation error (%4)
+ $signed($signed(a) << sha) #check again @ synthesis

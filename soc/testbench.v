// file: phrv32CPU_tb.v
// author: @shalan
// Testbench for rv32i_alu_opt_

`timescale 1ns/1ns

`define     OPCODE_Branch   5'b11_000
`define     OPCODE_Load     5'b00_000
`define     OPCODE_Store    5'b01_000
`define     OPCODE_JALR     5'b11_001
`define     OPCODE_JAL      5'b11_011
`define     OPCODE_Arith_I  5'b00_100
`define     OPCODE_Arith_R  5'b01_100
`define     OPCODE_AUIPC    5'b00_101
`define     OPCODE_LUI      5'b01_101

`define     F3_ADD          3'b000
`define     F3_SLL          3'b001
`define     F3_SLT          3'b010
`define     F3_SLTU         3'b011
`define     F3_XOR          3'b100
`define     F3_SRL          3'b101
`define     F3_OR           3'b110
`define     F3_AND          3'b111

//`define     _RDISP_	       0	
`define       _RDUMP_          0
`define       _SOCTEST_     0


module cpu_tb (
    input clk, rst,
    input[1:0] mode,    //00: reg read, 01: input, 10: output    
    input INTi,         //external interrupt (using switches)
    input[5:0] in,      //user input; {subset_in,rdy}; or rfSel
    output[7:0] Out,
    output [3:0] Y
    );


	wire[31:0] baddr, bdi, bdo;
	wire bwr;
	wire[1:0] bsz;

	wire[4:0] rfrd, rfrs1, rfrs2;
	wire rfwr;
	wire[31:0] rfD;
	wire[31:0] rfRS1, rfRS2, rfSelD;

	wire[31:0] extA, extB;
	wire[31:0] extR;
	wire extStart;
	wire extDone;
	wire[2:0] extFunc3;

	wire IRQ;
	wire [3:0] IRQnum;
	wire [15:0] IRQen;
	wire [15:0] INT = {15'd0,INTi}; 
	
	wire[31:0] disp_w;
	wire[31:0] output_w;

	IntCtrl INTCU(
		.clk(clk), .rst(rst),
		.INT(INT), .IRQen(IRQen),
		.IRQ(IRQ), .IRQnum(IRQnum)
		);

	rv32_CPU_v2 CPU(
		.clk(clk),
		.rst(rst),
		.bdi(bdi), .bdo(bdo), .baddr(baddr), .bsz(bsz), .bwr(bwr),
		.rfwr(rfwr), .rfrd(rfrd), .rfrs1(rfrs1), .rfrs2(rfrs2), .rfD(rfD), .rfRS1(rfRS1), .rfRS2(rfRS2),
		.extA(extA), .extB(extB), .extR(extR), .extStart(extStart), .extDone(extDone), .extFunc3(extFunc3),
		.IRQ(IRQ), .IRQnum(IRQnum), .IRQen(IRQen)
		);

    regFile RF(.clk(clk), .rst(rst),
               .rfwr(rfwr),	
               .rfrd(rfrd), .rfrs1(rfrs1), .rfrs2(rfrs2),
               .rfD(rfD), .rfRS1(rfRS1), .rfRS2(rfRS2)
        `ifdef _SOCTEST_
               ,.rfSel(in[4:0]),.rfSelD(rfSelD)
         `endif
               );
    
	memory #(4096) M (.clk(clk), .bdi(bdi), .baddr(baddr), .bdo(bdo), .bwr(bwr), .bsz(bsz) );
    
    inputDev ip(.di((mode == 2'b01)?in:6'b0), .baddr(baddr),.bdo(bdo)); 
    
    outputReg op (.clk(clk), .rst(rst), .bwr(bwr), .bdi(bdi), .baddr(baddr), .do(output_w)); 
    
    
    assign disp_w = (mode == 2'b01)? in       :
                    (mode == 2'b10)? output_w : 
               `ifdef _SOCTEST_
                    (mode == 2'b00)? rfSelD   : 
               `endif  
                    31'h11dead11;             
    display disp(.clk(clk), .rst(rst), .Num(disp_w),.Out(Out),.Y(Y));



    assign extR = 32'b0;
    assign extDone = 1'b1;
/*
	mul MULEXT (
		.clk(clk),
		.rst(rst),
		.done(extDone),
		.start(extStart),
		.a(extA), .b(extB),
		.p(extR)
		);
*/
endmodule


module outputReg (
    input clk, rst, bwr,
    input[31:0] bdi, baddr,
    output reg[31:0] do //no size
    ); 
    wire wr = bwr & (baddr == 32'h80000000); 
    always @ (posedge clk) begin
        if (rst)
            do <= 32'd0;
        else if (wr)
            do <= bdi;    
    end
endmodule

module inputDev ( //no regs
    input[5:0] di,
    input[31:0] baddr,
    output [31:0] bdo //no size
    ); 
    wire addr_targ = (baddr == 32'h80000001);
    assign bdo = addr_targ? {26'd0, di} : 32'hZZZZZZZZ;   
endmodule





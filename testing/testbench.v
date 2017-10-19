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

//`define     _RDISP_	    0	
`define     _RDUMP_       0



module cpu_tb;

	reg clk, rst;


	wire[31:0] baddr, bdi, bdo;
	wire bwr;
	wire[1:0] bsz;

	wire[4:0] rfrd, rfrs1, rfrs2;
	wire rfwr;
	wire[31:0] rfD;
	wire[31:0] rfRS1, rfRS2;
	wire simdone;

	wire[31:0] extA, extB;
	wire[31:0] extR;
	wire extStart;
	wire extDone;
	wire[2:0] extFunc3;

	wire IRQ;
	wire [3:0] IRQnum;
	wire [15:0] IRQen;
	reg [15:0] INT; //reg for sim

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
		.IRQ(IRQ), .IRQnum(IRQnum), .IRQen(IRQen),
		.simdone(simdone)
		);

	memory #(4096) M (.clk(clk), .bdi(bdi), .baddr(baddr), .bdo(bdo), .bwr(bwr), .bsz(bsz) );

	mul MULEXT (
		.clk(clk),
		.rst(rst),
		.done(extDone),
		.start(extStart),
		.a(extA), .b(extB),
		.p(extR)
		);

	// simulate the RF
    regFile RF(.clk(clk), .rst(rst),
               .rfwr(rfwr),	
               .rfrd(rfrd), .rfrs1(rfrs1), .rfrs2(rfrs2),
               .rfD(rfD), .rfRS1(rfRS1), .rfRS2(rfRS2)

			   ,.simdone(simdone)
               );

	integer i;


	initial begin clk = 0; end

	always # 5 clk = ~ clk;

	initial begin
		rst = 0;
		#50;
		@(negedge clk);
		rst = 1;
		#50;
		@(negedge clk);
		rst = 0;
	end

	// This to test external interruppts !
	initial begin
		INT = 16'd0;
		#300    INT[0] = 1'b1;
		#20     INT[0] = 1'b0;

		#300    INT[1] = 1'b1;
		#20     INT[1] = 1'b0;
	end

endmodule

module regFile( //parameterize
    input clk, rst,
    input rfwr,	
    input [4:0] rfrd, rfrs1, rfrs2,
    input [31:0] rfD,
    output [31:0] rfRS1, rfRS2

	,input simdone
    );
    reg[31:0] RF[31:0];

    assign rfRS1 = RF[rfrs1];
	assign rfRS2 = RF[rfrs2];


	integer i;
    initial begin //rst later
        for(i=0; i<32; i=i+1)
            RF[i] = 0;
    end
    
    always @(posedge clk)
        if(!rst) begin
            if(rfwr) begin
                RF[rfrd] <= rfD;
            end
        end

	`ifdef  _RDUMP_
		always @ (posedge simdone)
			//$display ("DONE!!!!");
			for(i=0; i<32; i=i+1)
				$display("x%0d: \t0x%h\t%0d",i,RF[i], $signed(RF[i]));
	`endif
    
endmodule

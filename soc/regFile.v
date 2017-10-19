`timescale 1ns / 1ps
`define       _SOCTEST_     0

module regFile( //parameterize
    input clk, rst,
    input rfwr,	
    input [4:0] rfrd, rfrs1, rfrs2,
    input [31:0] rfD,
    output [31:0] rfRS1, rfRS2
`ifdef _SOCTEST_
    ,input[4:0] rfSel,
    output [31:0] rfSelD
`endif
    );
    reg[31:0] RF[31:0];

    assign rfRS1 = RF[rfrs1];
	assign rfRS2 = RF[rfrs2];
`ifdef _SOCTEST_
	assign rfSelD = RF[rfSel];
`endif


	integer i;
    initial begin
        for(i=0; i<32; i=i+1)
            RF[i] = 0; //not needed for FPGAs (default power-on zeros); use reset instead?
    end
    /*
    //synch read
    always @(posedge clk) begin
        rfRS1 <= RF[rfrs1];
        rfRS2 <= RF[rfrs2];
`ifdef _SOCTEST_
        rfSelD <= RF[rfSel];    
`endif
    end
    */
    
    always @(posedge clk)
        if(!rst) begin
            if(rfwr) begin
                RF[rfrd] <= rfD;
            end
        end
    
endmodule

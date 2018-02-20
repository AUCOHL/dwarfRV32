`timescale 1ns / 1ps


module testbench;
    reg clk=0, rst=1, trigger_ext;
    reg [23:0] addr;
    wire [31:0] do;
    wire rdy;

    always begin
        clk = 1;
	   	#5;
        clk = 0;
	   	#5;
    end

    initial begin
        #20; 
        rst=0;
        #10;
    end

    rom_qspi  rq (
        .clk(clk), .rst(rst),
        .baddr(addr[23:0]),
        .bsz(2'b0),
        .trigger_rd(trigger_ext),   //remove ?
        .bdo(do),  
        .brdy(rdy)
    );

    initial begin
        addr = 0;
        #90;
        trigger_ext = 1;
        addr = 3;
        #1200         trigger_ext = 0;

        #1250;
        addr = 7;
    
    end
 

endmodule

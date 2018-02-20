`timescale 1ns / 1ps


module testbench();
    
    reg clock=0, reset=1, trigger_ext;
    reg [23:0] addr;
    wire [31:0] do;
    wire rdy;

    always begin
        clock = 1; #5;
        clock = 0; #5;
    end

    initial begin
       // $display("TEST START");
        #20; 
        reset=0;
        #10;
        //while(!LED[0])
         //   #10;
       // $display("TEST DONE");
       // $stop;
    end

    rom_qspi  rq (
        .clk(clock), .rst(reset),
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

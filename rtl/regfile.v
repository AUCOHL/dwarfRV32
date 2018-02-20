
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
`ifdef _DBUG_
				$display("x%0d = \t0x%h -> %h",rfrd,RF[rfrd], rfD);
`endif	
            end
        end

`ifdef _RDUMP_
		always @ (posedge simdone)
			//$display ("DONE!!!!");
			for(i=0; i<32; i=i+1)
				$display("x%0d: \t0x%h\t%0d",i,RF[i], $signed(RF[i]));
`endif
    
endmodule

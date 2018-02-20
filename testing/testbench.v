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
`define     INST_NOP        32'h13

//`define     _DBUG_	    0	
`define     _RDUMP_       0
//`define     _AHBL_        0


`ifndef _AHBL_
module cpu_tb_generic;

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

	wire brdy; //, rdy1, rdy2;     
	wire CS, CSM, CSQM; //, CS1, CS2; //to know if any of them is selected
	wire IRQ;
	wire [3:0] IRQnum;
	wire [15:0] IRQen;
	reg [15:0] INT; //reg for sim

	//if none is selected, the bus is ready
	assign CS = ~(CSM | CSQM); // ~(CSM|CS1|CS2)
	assign brdy = (CS)? 1'b1 : 1'bz; 

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
		.brdy(brdy),
		.IRQ(IRQ), .IRQnum(IRQnum), .IRQen(IRQen),
		.simdone(simdone)
	);

	m_wrapper #(12) M (
		.clk(clk),
		.baddr(baddr),
		.bdi(bdi),
		.bsz(bsz),
		.bwr(bwr),
		.bdo(bdo),
		.brdy(brdy),
		.CS(CSM)
	);

	qm_wrapper QM (
		.clk(clk),
		.rst(rst),
		.baddr(baddr),
		.bdi(bdi),
		.bsz(bsz),
		.bwr(bwr),
		.bdo(bdo),
		.brdy(brdy),
		.CS(CSQM)
	);

	mul MULEXT (
		.clk(clk),
		.rst(rst),
		.done(extDone),
		.start(extStart),
		.a(extA), .b(extB),
		.p(extR)
	);

	// simulate the RF
    regFile RF (.clk(clk), .rst(rst),
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
	   #300    INT[4] = 1'b1;
	   #20     INT[4] = 1'b0;

	   #300    INT[14] = 1'b1;
	   #20     INT[14] = 1'b0;
   end

`ifdef _DBUG_
   always @ (posedge clk) begin
	   $display("baddr_tb = %h, brdy_tb: %b, CS_tb : %b, bdo_tb = %h", baddr, brdy, CS, bdo);
   end
`endif

endmodule


module m_wrapper #(parameter logHBank = 10) (
	input clk,
	input [31:0] baddr,
	input [31:0] bdi,
	input [1:0] bsz,
	input bwr,
	output [31:0] bdo,
	output brdy,
	output CS
);

	wire [31:0] bdo_m;
	wire brdy_m;


`ifdef _DBUG_
	always @ (posedge clk)begin
		$display("baddr_m: %h, CSM = %b, bdo_m = %h, brdy_m = %b", baddr, CS, bdo_m, brdy_m);
	end

`endif
	assign CS = (baddr[31:logHBank+2] == 0);

	wire mwr = CS & bwr;

	memory #(logHBank) M (.clk(clk), .bdi(bdi), .baddr(baddr), .bdo(bdo_m), .mwr(mwr), .bsz(bsz), .brdy(brdy_m));

    assign bdo = CS? bdo_m : 32'hZZZZZZZZ;
	assign brdy = CS? brdy_m : 1'bz;

endmodule

//HEX fddddxxxxx
module qm_wrapper (
	input clk,
	input rst,
	input [31:0] baddr,
	input [31:0] bdi,
	input [1:0] bsz,
	input bwr,
	output [31:0] bdo,
	output brdy,
	output CS
);

	wire [31:0] bdo_m;
	wire brdy_m;

	assign CS = (baddr[31:28] == 4'hf); // first 4 bits are f

	wire mwr = CS & bwr;

    rom_qspi  rq (
        .clk(clk), .rst(rst),
        .baddr(baddr[23:0]),
        .bsz(2'b0),
        .trigger_rd(CS),   //remove ?
        .bdo(bdo_m),  
        .brdy(brdy_m)
    );

    assign bdo = CS? bdo_m : 32'hZZZZZZZZ;
	assign brdy = CS? brdy_m : 1'bz;
`ifdef _DBUG_
	always @ (posedge clk) begin
		$display("baddr_qm: %h, CSQM = %b, bdo_qm = %h, brdy_qm = %b", baddr, CS, bdo_m, brdy_m);
	end
`endif

endmodule

module io_wrapper #(parameter targ_addr = 32'h80000000) (
	input clk, rst,
	input [31:0] baddr,
	input [31:0] bdi,
	input [31:0] bdi_m,
	input [1:0] bsz,
	input bwr,
	output [31:0] bdo,
	input brdy_m,
	output brdy,
	output CS
);

	reg [31:0] ioreg;

	assign CS = (targ_addr == baddr);
	wire mwr = CS & bwr;

	always @ (posedge clk) begin
		if (rst)
			ioreg <= 32'b0;
		//priority to CPU
		else if (mwr)
			ioreg <= bdi;
		else if (brdy_m)
			ioreg <= bdi_m;
	end

	//if external data is provided during cpu read
    assign bdo = CS? (brdy_m ? bdi_m : ioreg) : 32'hZZZZZZZZ;
	assign brdy = CS? 1'b1 : 1'bz;

endmodule
`else

//////////////////////
//AHB LITE BUS SOC///
////////////////////

module cpu_tb_ahbl;
    //                      0               1     2     3..4..5..
    //number of devices: default wrapper + mem + qm + ioN -> (3+ioN)
    localparam ioN = 1;
    localparam selBitLn = 5;


    //HCLK
    reg HCLK, HRST;
   initial begin HCLK = 0; end

   always # 5 HCLK = ~ HCLK;

   initial begin
	   HRST = 0;
	   #50;
	   @(negedge HCLK);
	   HRST = 1;
	   #50;
	   @(negedge HCLK);
	   HRST = 0;
       $display("initialized");
   end   

    
    //MASTER
    wire [31:0] HADDR, HWDATA, HRDATA;
    wire [1:0]  HSIZE;
    wire HREADY, HTRANS, HWRITE;

    reg [15:0] INT; //reg for sim

    AHBLMASTER AHBM (
        .HCLK(HCLK), .HRST(HRST),
     
        .HRDATA(HRDATA),
        .HREADY(HREADY),
        //input           HRESP,                //for errors
        
        .HADDR(HADDR),
        .HSIZE(HSIZE),                          // [2:0]
        .HTRANS(HTRANS),                        //current transfer type
        .HWDATA(HWDATA),
        .HWRITE(HWRITE),
        
        
        //interrupts interface here
        .INT(INT)
    );
    
    //DECODER
    wire [0:2+ioN] HSEL; 
    AHBLDEC  #(12, ioN) AHBLD (
        .HADDR(HADDR),
        .HSEL(HSEL)
    );
    
    //SLAVES
    wire [0:2+ioN] HREADYout;
    wire [31:0] HRDATAout [0:2+ioN];
    //default wrapper (0)
    assign HREADYout[0] = 1'b1;
    assign HRDATAout[0] = 32'h40404040; //default wrapper "signature"
    
    //memory (1)
	AHBSLAVE_M #(12) M (
            .HSEL(HSEL[1]),

            .HCLK(HCLK),
            .HRST(HRST),

            .HREADY(HREADY),
			.HADDR(HADDR),
			.HTRANS(HTRANS),
			.HWRITE(HWRITE),
			.HSIZE(HSIZE),
			
            .HWDATA(HWDATA),

            .HREADYout(HREADYout[1]),
            .HRDATA(HRDATAout[1])
	
    );

    //qm (2)
    assign HREADYout[2] = 1'b1;
    assign HRDATAout[2] = `INST_NOP; //default wrapper "signature"
    

    //generate io
    AHBSLAVE_IO io1 (
        .HSEL(HSEL[3]),
        .HCLK(HCLK),
        .HRST(HRST),
        .HTRANS(HTRANS),
        .HWDATA(HWDATA),
        .HSIZE(HSIZE),                  // [2:0]
        .HWRITE(HWRITE),
        .HREADY(HREADY),
        
        .HREADYout(HREADYout[3]),
        .HRDATA(HRDATAout[3])
    );
    
    //MUX
    reg [selBitLn-1:0] HREADY_sel, HRDATA_sel;
    
    integer i;
    always@ (posedge HCLK, posedge HRST) begin
        if(HRST) begin
            HREADY_sel <= 0;
            HRDATA_sel <= 0;
        end
        else if(HREADY) begin
            for (i = 0; i < 3+ioN; i = i + 1)
                if (HSEL[i] & HTRANS) begin
                    HREADY_sel <= i[selBitLn-1:0];
                    HRDATA_sel <= i[selBitLn-1:0];
                end
        end
    end
    assign HREADY = HREADYout[HREADY_sel];
    assign HRDATA = HRDATAout[HRDATA_sel];


   // This to test external interruppts !
/*
   initial begin
	   INT = 16'd0;
	   #300    INT[4] = 1'b1;
	   #20     INT[4] = 1'b0;

	   #300    INT[14] = 1'b1;
	   #20     INT[14] = 1'b0;
   end
*/

`ifdef _DBUG_
    always @ *
        $display ("HRDATA_tb = %h", HRDATA);
    always @ *
        $display ("HRDATA_sel = %h", HRDATA_sel);
`endif

endmodule

module AHBLMASTER (
    input HCLK, HRST,

    input [31:0]    HRDATA,
    input           HREADY,
    //input           HRESP,                //for errors

    output [31:0]   HADDR,
    output [1:0]    HSIZE,                  // [2:0]
    output          HTRANS,                 //current transfer type
    output [31:0]   HWDATA,
    output          HWRITE,


    //external interrupts interface here
    input [15:0] INT
);

    localparam IDLE     = 1'b0;             // no transfer needed
    localparam NONSEQ   = 1'b1;             // single / burst initial 
    
    localparam SZ_BYTE = 2'b00;
    localparam SZ_HW   = 2'b01;
    localparam SZ_W    = 2'b10;

    //localparam BUSY     = 2'b01;          //for burst
    //localparam SEQ      = 2'b11;
    
    //cpu here driving HWDATA and HADDR, HSIZE, HWRITE, .brdy(HREADY), 
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


    rv32_CPU_v2 CPU(
		.clk(HCLK),
		.rst(HRST),
		.bdi(HWDATA), .bdo(HRDATA), .baddr(HADDR), .bsz(HSIZE), .bwr(HWRITE),
		.brdy(HREADY),

		.rfwr(rfwr), .rfrd(rfrd), .rfrs1(rfrs1), .rfrs2(rfrs2), .rfD(rfD), .rfRS1(rfRS1), .rfRS2(rfRS2),

		.extA(extA), .extB(extB), .extR(extR), .extStart(extStart), .extDone(extDone), .extFunc3(extFunc3),

		.IRQ(IRQ), .IRQnum(IRQnum), .IRQen(IRQen),
		.simdone(simdone)
	);
    
    //to rework if needed
    assign HTRANS = NONSEQ;    

	IntCtrl INTCU(
		.clk(HCLK), .rst(HRST),
		.INT(INT), .IRQen(IRQen),
		.IRQ(IRQ), .IRQnum(IRQnum)
	);

	mul MULEXT (
		.clk(HCLK),
		.rst(HRST),
		.done(extDone),
		.start(extStart),
		.a(extA), .b(extB),
		.p(extR)
	);

	// simulate the RF
    regFile RF (.clk(HCLK), .rst(HRST),
               .rfwr(rfwr),	
               .rfrd(rfrd), .rfrs1(rfrs1), .rfrs2(rfrs2),
               .rfD(rfD), .rfRS1(rfRS1), .rfRS2(rfRS2)

			   ,.simdone(simdone)
   );
    
      
    //extensions here

endmodule

module AHBSLAVE_M #(parameter logHBank = 10)
(
			input HSEL,

			input HCLK,
			input HRST,

			input HREADY,
			input [31:0] HADDR,
			input HTRANS,
			input HWRITE,
			input [1:0] HSIZE,
			
			input [31:0] HWDATA,

			output HREADYout,
			output [31:0] HRDATA
	
);


  //assign HREADYout = 1'b1;



  reg last_HSEL, last_HWRITE, last_HTRANS;
  reg [31:0] last_HADDR;
  reg [1:0] last_HSIZE;

 
  always @(posedge HCLK, posedge HRST)
  begin
	 if(HRST)
	 begin
		last_HSEL <= 1'b0;
        last_HWRITE <= 1'b0;
        last_HTRANS <= 1'b1;
		last_HADDR <= 32'd0;
		last_HSIZE <= 2'b0;
	 end
    else if(HREADY)
    begin
        last_HSEL <= HSEL;
        last_HWRITE <= HWRITE;
        last_HTRANS <= HTRANS;
		last_HADDR <= HADDR;
		last_HSIZE <= HSIZE;
    end
  end

  reg [31:0] baddr_buffer;  //prevent "accidental" selection (since the memory module doesn't have a CS)
  always @ (*)
    if (last_HSEL)
        baddr_buffer = last_HADDR;

  memory #(logHBank) M (.clk(HCLK), .bdi(HWDATA), .baddr(baddr_buffer), .bdo(HRDATA), .mwr(last_HWRITE & last_HTRANS), .bsz(last_HSIZE), .brdy(HREADYout));
`ifdef _DBUG_
    always @ *
        $display("HRDATA_memory = %h", HRDATA);
    always @ *
        $display("baddr_buffer = %h", baddr_buffer);
    always @ *
        $display("last_HSEL = %h", last_HSEL);
`endif

endmodule

module AHBSLAVE_IO (
    input               HSEL,
    input               HCLK,
    input               HRST,
    input               HTRANS,
    input  [31:0]       HWDATA,
    input  [1:0]        HSIZE,                  // [2:0]
    input               HWRITE,
    input               HREADY,
    
    
    output              HREADYout,
    output  [31:0]      HRDATA,

    output reg [4:0]    strg
);
	 

  reg last_HSEL;
  reg last_HWRITE;
  reg last_HTRANS;

  always@ (posedge HCLK)
  begin
    if(HREADY)
    begin
      last_HSEL     <= HSEL;
      last_HWRITE   <= HWRITE;
      last_HTRANS   <= HTRANS;
    end
  end
    
  always@ (posedge HCLK, posedge HRST)
  begin
    if(HRST)
      strg <= 4'd0;
    else if(last_HSEL & last_HWRITE & last_HTRANS)
      strg <= HWDATA[4:0];
  end


  assign HREADYout = 1'b1;
  assign HRDATA = strg;

endmodule

module AHBLDEC #(parameter logHBank = 10, parameter ioN = 1) (
	input [31:0] HADDR,
	output reg [0:2+ioN] HSEL
);
    reg [0:1+ioN] HSEL_wo_def;
    integer i;
    always @ * begin
    	HSEL_wo_def[0] = (HADDR[31:logHBank+2] == 0);		        //0x[000]*xxxxx
		HSEL_wo_def[1] = (HADDR[31:28] == 4'hf);			        //0xfddddxxxxx
    	for (i = 0; i < ioN; i = i + 1)                             //0x8000000x
    	    HSEL_wo_def[i+2] = (HADDR == {28'h8000000, i[3:0]});
    	    
    	if (HSEL_wo_def == 0)
    	    HSEL = {1'b1, HSEL_wo_def};
    	else
    	    HSEL = {1'b0, HSEL_wo_def};
    end

endmodule



`endif


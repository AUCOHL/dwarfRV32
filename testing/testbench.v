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




module cpu_tb;

reg clk, rst;


wire[31:0] addr_, cpu_do, cpu_di;
wire wr_;
wire[1:0] sz_;

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

reg eint;

//port names updated, but signal/wire names are given arbitrary names
 rv32_CPU_v2 CPU(
              .clk(clk),
              .rst(rst),
              .bdo(cpu_do), .bdi(cpu_di), .baddr(addr_), .bsz(sz_), .bwr(wr_),
              .rfwr(rfwr), .rfrd(rfrd), .rfrs1(rfrs1), .rfrs2(rfrs2), .rfD(rfD), .rfRS1(rfRS1), .rfRS2(rfRS2),
              .extA(extA), .extB(extB), .extR(extR), .extStart(extStart), .extDone(extDone), .extFunc3(),
              .eint(eint), .eint_num(4'b0000),
              .simdone(simdone)
              );

memory M (.clk(clk), .bdi(cpu_do), .baddr(addr_), .bdo(cpu_di), .bwr(wr_), .bsz(sz_) );

 mul MULEXT (
      .clk(clk),
      .rst(rst),
      .done(extDone),
      .start(extStart),
      .a(extA), .b(extB),
      .p(extR)
      );

// simulat ethe RF
reg[31:0] RF[31:0];

assign rfRS1 = RF[rfrs1];
assign rfRS2 = RF[rfrs2];

always @(posedge clk)
  if(!rst)
    if(rfwr) begin
      RF[rfrd] <= rfD;
      $display ("writing %0d to x%0d", rfD, rfrd);
    end

integer i;

initial begin
  for(i=0; i<32; i=i+1)
    RF[i] = 0;
end

//integer i;
//initial $monitor("SimDone: %d",simdone);

always @ (posedge simdone)
  //$display ("DONE!!!!");
    for(i=0; i<32; i=i+1)
        $display("x%0d:\t0x%h\t%0d",i,RF[i], $signed(RF[i]));

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
    eint = 0;
    #300    eint = 1;
    #20     eint = 0;
end

endmodule

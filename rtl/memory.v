// file: memory.v
// author: @shalan

module rv32i_mem_ctrl (addr, sz, cpuDo, memDi, cs, cpuDi, memDo);
    input[1:0] addr;
    input[1:0] sz;
    input[31:0] cpuDo, memDo;
    output reg [31:0] memDi, cpuDi;
    output reg [3:0] cs;

    wire szB=(sz==2'b0);
    wire szH=(sz==2'b01);
    wire szW=(sz==2'b10);

    always @ * begin
    (* full_case *)
        (* parallel_case *)
        case ({addr, szB, szH, szW})
            5'b00_001: begin memDi = cpuDo; cs=4'b1111; end

            5'b00_010: begin memDi = cpuDo; cs=4'b0011; end
            5'b10_010: begin memDi = cpuDo << 16; cs=4'b1100; end

            5'b00_100: begin memDi = cpuDo; cs=4'b0001; end
            5'b01_100: begin memDi = cpuDo << 8; cs=4'b0010; end
            5'b10_100: begin memDi = cpuDo << 16; cs=4'b0100; end
            5'b11_100: begin memDi = cpuDo << 24; cs=4'b1000; end
        endcase

    end

    always @ * begin
    (* full_case *)
        (* parallel_case *)
        case ({addr, szB, szH, szW})
            5'b00_001: cpuDi = memDo;

            5'b00_010: cpuDi = memDo;
            5'b10_010: cpuDi = memDo >> 16;

            5'b00_100: cpuDi = memDo;
            5'b01_100: cpuDi = memDo >> 8;
            5'b10_100: cpuDi = memDo >> 16;
            5'b11_100: cpuDi = memDo >> 24;
        endcase
    end

endmodule


module memory (input clk, input[31:0] di, addr, output[31:0] do, input wr, input[1:0] sz );
    reg[7:0] bank0[1023:0];
    reg[7:0] bank1[1023:0];
    reg[7:0] bank2[1023:0];
    reg[7:0] bank3[1023:0];

    // a dummy array to load the memory from th efile.
    reg[31:0] mem[1023:0];

    wire[31:0] memDo, memDi;
    wire[3:0] cs;

    rv32i_mem_ctrl MCTRL (.addr(addr[1:0]), .sz(sz), .cpuDo(di), .memDi(memDi), .cs(cs), .cpuDi(do), .memDo(memDo));

    assign memDo = {bank3[addr[11:2]],bank2[addr[11:2]],bank1[addr[11:2]],bank0[addr[11:2]]};

    always @ (posedge clk)
        if(cs[0] & wr)
            bank0[addr[11:2]] <= memDi[7:0];
    always @ (posedge clk)
        if(cs[1] & wr)
            bank1[addr[11:2]] <= memDi[15:8];
    always @ (posedge clk)
        if(cs[2] & wr)
            bank2[addr[11:2]] <= memDi[23:16];
    always @ (posedge clk)
        if(cs[3] & wr)
            bank3[addr[11:2]] <= memDi[31:24];


    // sim only
    always @ (posedge clk)
        if(cs[0] & wr)
          $display("writing %0d(%0h) to (%0d)%h -- Size:%0d", memDi, memDi, addr, addr,sz);


    integer i;

    initial begin
      $readmemh("./test.hex", mem);

  for(i=0; i<1024; i=i+1) begin
        {bank3[i], bank2[i], bank1[i], bank0[i]} = mem[i];
        //$display("bank1[%d]=%h\n", i, bank0[i]);
  end

  end

endmodule

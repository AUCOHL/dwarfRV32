// file: memory.v
// author: @shalan

module rv32i_mem_ctrl (baddr, bsz, bdi, mdi, mcs, bdo, mdo);
    input[1:0] baddr;
    input[1:0] bsz;
    input[31:0] bdi, mdo;
    output reg [31:0] mdi, bdo;
    output reg [3:0] mcs;

    wire szB=(bsz==2'b0);
    wire szH=(bsz==2'b01);
    wire szW=(bsz==2'b10);

    always @ * begin
    (* full_case *)
        (* parallel_case *)
        case ({baddr, szB, szH, szW})
            5'b00_001: begin mdi = bdi; mcs=4'b1111; end

            5'b00_010: begin mdi = bdi; mcs=4'b0011; end
            5'b10_010: begin mdi = bdi << 16; mcs=4'b1100; end

            5'b00_100: begin mdi = bdi; mcs=4'b0001; end
            5'b01_100: begin mdi = bdi << 8; mcs=4'b0010; end
            5'b10_100: begin mdi = bdi << 16; mcs=4'b0100; end
            5'b11_100: begin mdi = bdi << 24; mcs=4'b1000; end
        endcase

    end

    always @ * begin
    (* full_case *)
        (* parallel_case *)
        case ({baddr, szB, szH, szW})
            5'b00_001: bdo = mdo;

            5'b00_010: bdo = mdo;
            5'b10_010: bdo = mdo >> 16;

            5'b00_100: bdo = mdo;
            5'b01_100: bdo = mdo >> 8;
            5'b10_100: bdo = mdo >> 16;
            5'b11_100: bdo = mdo >> 24;
        endcase
    end

endmodule


module memory (input clk, input[31:0] bdi, baddr, output[31:0] bdo, input bwr, input[1:0] bsz );
    reg[7:0] bank0[1023:0];
    reg[7:0] bank1[1023:0];
    reg[7:0] bank2[1023:0];
    reg[7:0] bank3[1023:0];

    // a dummy array to load the memory from th efile.
    reg[31:0] mem[1023:0];

    wire[31:0] mdo, mdi;
    wire[3:0] mcs;

    rv32i_mem_ctrl MCTRL (.baddr(baddr[1:0]), .bsz(bsz), .bdi(bdi), .mdi(mdi), .mcs(mcs), .bdo(bdo), .mdo(mdo));

    assign mdo = {bank3[baddr[11:2]],bank2[baddr[11:2]],bank1[baddr[11:2]],bank0[baddr[11:2]]};

    always @ (posedge clk)
        if(mcs[0] & bwr)
            bank0[baddr[11:2]] <= mdi[7:0];
    always @ (posedge clk)
        if(mcs[1] & bwr)
            bank1[baddr[11:2]] <= mdi[15:8];
    always @ (posedge clk)
        if(mcs[2] & bwr)
            bank2[baddr[11:2]] <= mdi[23:16];
    always @ (posedge clk)
        if(mcs[3] & bwr)
            bank3[baddr[11:2]] <= mdi[31:24];


    // sim only
    always @ (posedge clk)
        if(mcs[0] & bwr)
          $display("writing %0d(%0h) to (%0d)%h -- Size:%0d", mdi, mdi, baddr, baddr,bsz);


    integer i;

    initial begin
      $readmemh("./test.hex", mem);

  for(i=0; i<1024; i=i+1) begin
        {bank3[i], bank2[i], bank1[i], bank0[i]} = mem[i];
        //$display("bank1[%d]=%h\n", i, bank0[i]);
  end

  end

endmodule

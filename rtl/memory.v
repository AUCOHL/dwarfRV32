// file: memory.v
// author: @shalan


//`define _MEMDISP_ 	0

//MM IO
`define    MMAP_PRINT	32'h80000000
`define    LOGCAPH	14 

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


module memory #(parameter capH = 1024) (input clk, input[31:0] bdi, baddr, output[31:0] bdo, input bwr, input[1:0] bsz );
    reg[7:0] bank0[capH-1:0];
    reg[7:0] bank1[capH-1:0];
    reg[7:0] bank2[capH-1:0];
    reg[7:0] bank3[capH-1:0];

    // a dummy array to load the memory from th efile.
    reg[31:0] mem[capH-1:0];

    wire[31:0] mdo, mdi;
    wire[3:0] mcs;

    rv32i_mem_ctrl MCTRL (.baddr(baddr[1:0]), .bsz(bsz), .bdi(bdi), .mdi(mdi), .mcs(mcs), .bdo(bdo), .mdo(mdo));

    assign mdo = {bank3[baddr[`LOGCAPH:2]],bank2[baddr[`LOGCAPH:2]],bank1[baddr[`LOGCAPH:2]],bank0[baddr[`LOGCAPH:2]]};
    always @(posedge clk) begin
	    if (bwr) begin
		    case(baddr)
			    `MMAP_PRINT: begin
			    	$write("%c", mdi[7:0]);
				$fflush(); 
			    end
			    default: begin
				if(mcs[0]) bank0[baddr[`LOGCAPH:2]] <= mdi[7:0];
				if(mcs[1]) bank1[baddr[`LOGCAPH:2]] <= mdi[15:8];
				if(mcs[2]) bank2[baddr[`LOGCAPH:2]] <= mdi[23:16];
				if(mcs[3]) bank3[baddr[`LOGCAPH:2]] <= mdi[31:24];
			    end
		    endcase
	    end
	    

    end

    // sim only
`ifdef _MEMDISP_
    always @ (posedge clk)
        if(mcs[0] & bwr)
          $display("writing %0d(%0h) to (%0d)%h -- Size:%0d", mdi, mdi, baddr, baddr,bsz);
`endif


    integer i;

    initial begin
      $readmemh("./test.hex", mem);

  for(i=0; i<capH; i=i+1) begin
        {bank3[i], bank2[i], bank1[i], bank0[i]} = mem[i];
        //$display("bank1[%d]=%h\n", i, bank0[i]);
  end

  end

endmodule

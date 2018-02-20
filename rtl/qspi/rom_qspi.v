`define CMD_WREN 8'h06
`define CMD_WRSR 8'h01		//0x0200
`define CMD_FQIO 8'heb		//add dummy 2 bytes in address

//to verify communication
`define CMD_RDID 8'hAB		//followed by 3 dummy bytes
`define DEV_ID	//???

module rom_qspi (
    input clk, rst,
    input [23:0] baddr,
    input [1:0] bsz,   //always a word for now    
    input trigger_rd, //remove ?
    output [31:0] bdo,
    output brdy
);
    
    wire [31:0] bdo;


    wire clk_to_mem, CS, DQ3, DQ2, DQ1, DQ0, OE, bbusy;

    assign brdy = ~bbusy;
    hw_fsm ht( .rst(rst), .clk_100m(clk), .clk_to_mem_out(clk_to_mem), 
                .CS(CS),  .DQio({DQ3, DQ2, DQ1, DQ0}), .OE(OE), .trigger_rd(trigger_rd), .addr(baddr), .readout(bdo), .busy_out(bbusy));
    

    //model here/flash rom here
    reg [31:0] mem[31:0];
    assign {DQ3, DQ2, DQ1, DQ0} = OE? 4'bzzzz : mem[baddr][3:0];
    initial begin
		mem[0] = 32'd0;
		mem[1] = 32'd1;
		mem[2] = 32'd2;
		mem[3] = 32'd3;
		mem[4] = 32'd4;
		mem[5] = 32'd5;
		mem[6] = 32'd6;
		mem[7] = 32'd7;
		mem[8] = 32'd8;
		mem[9] = 32'd9;
		mem[10] = 32'd10;
		mem[11] = 32'd11;
		mem[12] = 32'd12;
		mem[13] = 32'd13;
		mem[14] = 32'd14;
		mem[15] = 32'd15;
		mem[16] = 32'd16;
		mem[17] = 32'd17;
		mem[18] = 32'd18;
		mem[19] = 32'd19;
		mem[20] = 32'd20;
		mem[21] = 32'd21;
		mem[22] = 32'd22;
		mem[23] = 32'd23;
		mem[24] = 32'd24;
		mem[25] = 32'd25;
		mem[26] = 32'd26;
		mem[27] = 32'd27;
		mem[28] = 32'd28;
		mem[29] = 32'd29;
		mem[30] = 32'd30;
		mem[31] = 32'd31;
    end
endmodule


module hw_fsm(
        input clk_100m,
        output clk_to_mem_out,
        input rst,
        input trigger_rd,
        input [23:0] addr,
        output [31:0] readout,
        inout [3:0] DQio,
        output CS,
        output OE, //remove
        output busy_out
    );
   
    wire clk_to_mem, clk;
    wire EOS;

    assign clk_to_mem_out = clk_to_mem;

    wire busy;
    wire error;
    reg trigger;
    reg QE;
    reg [7:0] cmd;
    reg [31:0] data_send;
    reg [4:0] state;
    reg blink;
    reg cnt;
    
 
    assign clk = clk_100m;
    assign clk_to_mem = clk_100m;
    //READY STATE
    assign busy_out = (state != 6) || busy; 

    //clk generator here
	//
	qspi_mem_controller mc(
        .clk(clk), 
        .reset(rst),
        .CS(CS), 
        .DQio(DQio),
        .trigger(trigger),
        .QE(QE),
        .cmd(cmd),
        .data_send(data_send),
        .readout(readout),
        .busy(busy),
        .error(error), .OE(OE));

	//INIT FSM
    always @(posedge clk) begin
        if(rst) begin
            trigger <= 0;
            state <= 0;
            blink <= 0;
            QE <= 0;
            cnt <= 0;
        end else begin
            blink <= ~blink;
        
            case(state)
                0: begin
                    if(!busy)
                        state <= state+1;
                end
                
                1: begin    // read memory ID to check communication (find ID later from the doc)
                    cmd <= `CMD_RDID;
                    trigger <= 1;   
                    state <= state+1;                    
                end
                
                2: begin    //Enable Writing
                    if(trigger)
                        trigger <= 0;
                    else if(!busy) begin
                        QE <= 1;
                        cmd <= `CMD_WREN;
                        trigger <= 1;   
                        state <= state+1;    
                    end
                end

                3: begin    //Write the Status Register to enable the Quad Protocol for QSPI
                    if (trigger)
                        trigger <= 0;
                    else if(!busy) begin
                        if (/*readout == `DEV_ID*/1) begin // verify the memory ID read

                            // enable quad IO
                            cmd <= `CMD_WRSR;
                            data_send[15:0] <= 16'h0200;  // quad protocol, hold/accelerator disabled, default drive strength
                            QE <= 1;
                            trigger <= 1;   
                            state <= state+1;  
                        end 
                    end
                end
                
                //MAIN STATE
                4: begin 
                    if(trigger)
                        trigger <= 0;
                    else if (!busy && trigger_rd) begin //this is to make the rdy signal HIGH only after the first trigger
                        cnt <= 1'b0;
                        cmd <= `CMD_FQIO;
                        data_send[31:0] <= {addr, 8'b0};
                        trigger <= 1;  
                        state <= state+1;
                    end
                end
                
                //intermediate wait states
                5: begin 
                    if(trigger)
                        trigger <= 0;
                    else if (!busy)
                        state <= state+1;
                end
                
                //OUTPUT READY STATE
                6: begin    //Read data in quad mode
                    cnt <= cnt + 1;
                    if (cnt == 1'b1)
                        state <= state-2;
                end
                
                default:
                    state <= state+1;
            endcase
        end
    end

endmodule





module qspi_mem_controller(
        input clk,
        input reset,
        input trigger,
        input QE,
        input [7:0] cmd,
        input [31:0] data_send, //max: 256B page data + 3B address
        output reg [31:0] readout,
        output reg busy,
        output reg error,

        inout [3:0] DQio,
        output CS,
        output OE //remove
    );
    parameter STATE_IDLE = 0;
    parameter STATE_WAIT = 1;
    parameter STATE_WREN = 2;
    parameter STATE_RDID = 3;
    parameter STATE_WRSR = 4;
    parameter STATE_FQIO = 5;
    
    
    reg spi_trigger;
    wire spi_busy;
    
    reg [55:0] data_in; //7 bytes max
    reg [2:0] data_in_count, data_out_count;
    wire [31:0] data_out;

    spi_cmd sc(.clk(clk), .reset(reset), .trigger(spi_trigger), .busy(spi_busy), .QE(QE),
        .data_in_count(data_in_count), .data_out_count(data_out_count), .data_in(data_in), .data_out(data_out),
        .DQio(DQio[3:0]), .CS(CS), .OE(OE));
    
    reg [5:0] state;
    reg [5:0] nextstate;
    
    always @(posedge clk) begin
        if(reset) begin
            state <= STATE_WAIT;
            nextstate <= STATE_IDLE;
            spi_trigger <= 0;
            busy <= 1;
            error <= 0;
            readout <= 0;
        end
        
        else
            case(state)
                STATE_IDLE: begin
                    if(trigger) begin
                        busy <= 1;
                        error <= 0;
                        case(cmd)
                            `CMD_WREN:
                                state <= STATE_WREN;
							`CMD_WRSR:
                                state <= STATE_WRSR;
							`CMD_FQIO:
                                state <= STATE_FQIO;
                            `CMD_RDID:
								state <= STATE_RDID;

                            default: begin
                                $display("ERROR: unknown command!");
                                $display(cmd);
                                $stop;
                            end
                        endcase
                    end else
                        busy <= 0;
                end
				
                STATE_RDID: begin
                    data_in <= {`CMD_RDID, 24'd0};
                    data_in_count <= 4;
                    data_out_count <= 1;
                    spi_trigger <= 1;
                    state <= STATE_WAIT;
                    nextstate <= STATE_IDLE;
                    if (QE) begin
                        $display("ERROR: RDID is not available in quad mode!");
                        $stop;
                    end
				end 

            
                STATE_FQIO: begin
                    data_in <= {`CMD_FQIO, data_send[31:0], 16'd0};
                    data_in_count <= 7;
                    data_out_count <= 4;    //control bsz here
                    spi_trigger <= 1;
                    state <= STATE_WAIT;
                    nextstate <= STATE_IDLE;
                    if (!QE) begin
                        $display("ERROR: Quad Mode must be enabled first!");
                        $stop;
                    end
                end                

                STATE_WREN: begin
                    data_in <= `CMD_WREN;
                    data_in_count <= 1;
                    data_out_count <= 0;
                    spi_trigger <= 1;
                    state <= STATE_WAIT;
                    nextstate <= STATE_IDLE;
                end
				
				STATE_WRSR: begin
                    data_in <= {`CMD_WRSR, data_send[15:0]};
                    data_in_count <= 3;
                    data_out_count <= 0;
                    spi_trigger <= 1;
                    state <= STATE_WAIT;
                    nextstate <= STATE_IDLE;
				end

                STATE_WAIT: begin
                    spi_trigger <= 0;
                    if (!spi_trigger && !spi_busy) begin
                        state <= nextstate;
                        readout <= data_out;
                    end
                end
            endcase
    end
endmodule






//Data sender/receiver to/from the device
module spi_cmd(
        //control interface
        input clk,
        input reset,
        input trigger,
        output reg busy,
        input [2:0] data_in_count,
        input [2:0] data_out_count,
        input [55:0] data_in, 
        output reg [31:0] data_out,
        input QE,
        output OE, //remove
        //SPI interface
        inout [3:0] DQio,
        output reg CS 
    );
    
    parameter STATE_IDLE = 0;
    parameter STATE_SEND = 1;
    parameter STATE_READ = 2;
    
    
    reg [11:0] base_addr;

    reg [3:0] DQ = 4'b1111;

    reg OE;
    assign DQio[0] = OE?DQ[0]:1'bZ;
    assign DQio[1] = OE?DQ[1]:1'bZ;
    assign DQio[2] = OE?DQ[2]:1'bZ;
    assign DQio[3] = QE?(OE?DQ[3]:1'bZ):1'b1; 
	

	//FSM:  IDLE -> if triggered and -> SEND instruction -> READ data
    
    reg [1:0] state;    
	reg [3:0] instCnt;
	wire instPhase = ~instCnt[3];
    wire [2:0] width = instPhase? 1 : (QE?4:1);
	
    
     always @(posedge clk) begin
        if(reset) begin
            state <= STATE_IDLE;
            OE <= 0;
            instCnt <= 0;
            CS <= 1;
            busy <= 1;
        end else begin
            
            case(state)
                STATE_IDLE: begin
                    if(trigger && !busy) begin
                        state<=STATE_SEND;
						instCnt <= 0;
                        busy <= 1;
                        base_addr <= data_in_count*8 - 1;   
                     end else begin
                        CS <= 1;
                        busy <= 0;
                     end
                 end

                STATE_SEND: begin
                    CS <= 0;
                    OE <= 1;
                    if(QE && !instPhase) begin
                        DQ[0] <= data_in[base_addr-3];
                        DQ[1] <= data_in[base_addr-2];
                        DQ[2] <= data_in[base_addr-1];
						DQ[3] <= data_in[base_addr];
                    end else
						DQ[0] <= data_in[base_addr];
                    
					if (instPhase)
						instCnt <= instCnt + 1;

                    if(base_addr>width-1) begin
                        base_addr <= base_addr - width;
                    end else begin
                        if(data_out_count>0) begin
                            state <= STATE_READ;
							data_out <= 0;
                            base_addr <= data_out_count*8; //because read happens on falling edge ///!!!
                        end
                        else begin
                            state <= STATE_IDLE;
                        end
                    end
                end

                STATE_READ: begin
                    OE <= 0;
                    
                    if(base_addr>width-1) begin
                        base_addr <= base_addr - width;
                    end else begin
                        CS <= 1;
                        state <= STATE_IDLE;
                    end
                end
                
                
                default: begin
              
                end
            endcase
        end
    end 
	
 	//no sign extension, byte/half-word read as is	
    always @(negedge clk) begin
        if(reset)
            data_out <= 0;
        else
            if(state==STATE_READ) begin
                if(QE)
                    data_out <= {data_out[27:0], DQio[3], DQio[2], DQio[1], DQio[0]};
                else
                    data_out <= {data_out[30:0], DQio[1]};
            end
    end

    
endmodule

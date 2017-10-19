`timescale 1ns / 1ps


module display(         //unnecessarily fancy
    input clk, rst,
    input [31:0] Num,
    output[7:0] Out,
    output reg[3:0] Y
    );

    wire [3:0] In;
    wire [15:0] Num_subset;
    reg [1:0] ScanCount;
    reg [2:0] SlideCount; reg upcount;

    SevSegDec hexDec (In, Out[6:0]);
    
    //decimal point
    assign Out[7] =  ~((ScanCount == 2'd0) && (SlideCount == 3'd0) || (ScanCount == 2'd3) && (SlideCount == 3'd4));
    
    reg gclk100; reg[18:0] gcount100;   // > 100Hz
    reg gclk1; reg[25:0] gcount1;       //1-2Hz
    always @ (posedge clk) begin 
        if (rst) begin
            gcount1 <= 26'd0;
            gcount100 <= 19'd0;
            gclk1 <= 1'b0;
            gclk100 <= 1'b0;
        end
        else begin
            if (gcount1 == 26'd12_500_000) begin //parameterize
                gcount1 <=  26'd0;
                gclk1 <= ~gclk1;
            end
            else
                gcount1 <= gcount1 + 1;
            if (gcount100 == 19'd312_500) begin
                gcount100 <= 19'd0;
                gclk100 <= ~gclk100;
            end
            else
                gcount100 <= gcount100 + 1;
        end
    end

    always @ (posedge gclk100) begin
        if (rst) begin              //refactor
            Y <= 4'b0111; // Y = {Y[0], Y[3:1]}
            ScanCount <= 2'b0;
        end
        else begin
            Y <= {Y[0], Y[3:1]};
            ScanCount <= ScanCount + 1;
        end
    end
    
    always @(posedge gclk1) begin
        if (rst) begin              //refactor
            upcount <= 1'b1;
            SlideCount <= 3'd0;
        end
        else begin 
            if (upcount) begin
                if (SlideCount == 3'd4)
                    upcount <= 1'b0;
                else
                    SlideCount <= SlideCount + 1;
            end
            else begin
                if (SlideCount == 3'd0)
                    upcount <= 1'b1;
                else
                    SlideCount <= SlideCount - 1;
            end
        end
    end
    
    assign Num_subset = (SlideCount == 3'd0)? Num[31:16] :
                    (SlideCount == 3'd1)? Num[27:12] :
                    (SlideCount == 3'd2)? Num[23:8]  :
                    (SlideCount == 3'd3)? Num[19:4]  : Num[15:0];                    
                                                      
    assign In = (ScanCount == 2'd0)? Num_subset[15:12]:
                (ScanCount == 2'd1)? Num_subset[11:8] :
                (ScanCount == 2'd2)? Num_subset[7:4]  : Num_subset[3:0]; 
    

endmodule



module SevSegDec (input [3:0] In, output reg [6:0] Out);
//assuming the order a,b,c,d,e,f,g
always @ (In) begin
	case (In)
		4'd0:	Out = 7'b000_0001; //decimal
		4'd1:	Out = 7'b100_1111;
		4'd2:	Out = 7'b001_0010;
		4'd3:	Out = 7'b000_0110;
		4'd4:	Out = 7'b100_1100;
		4'd5:	Out = 7'b010_0100;
		4'd6:	Out = 7'b010_0000;
		4'd7:	Out = 7'b000_1111;
		4'd8:	Out = 7'b000_0000;
		4'd9:	Out = 7'b000_0100;
		4'd10:	Out = 7'b000_1000; // for hex
		4'd11:	Out = 7'b110_0000;
		4'd12:	Out = 7'b011_0001;
		4'd13:	Out = 7'b100_0010;
		4'd14:	Out = 7'b011_0000;		
		4'd15:	Out = 7'b011_1000; 
		default: Out = 7'b111_1111;
	endcase
end
endmodule
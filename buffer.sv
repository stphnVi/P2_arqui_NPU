module buffer #(parameter ADDR=32, parameter DATA=32)(clk, rst, we, index, din, dout);

  input clk;
  input rst;
  input we; // Write enable: 1->write 0->read
  input      [ADDR-1:0] index;
  input      [DATA-1:0]       din;
  output reg [DATA-1:0]       dout;

  integer i;

  parameter DEPTH = 2**ADDR;


  // reg [`GBUFF_ADDR_SIZE-1:0] gbuff [`WORD_SIZE-1:0];
  reg [DATA-1:0] gbuff [DEPTH-1:0];


  always @ (negedge clk or negedge rst) begin
    if(!rst)begin
      for(i=0; i<(DEPTH); i=i+1)
        gbuff[i] <= 'd0;
    end
    else begin
      if(we) begin
        gbuff[index] <= din;
      end
      else begin
        dout <= gbuff[index];
      end
    end
  end

endmodule

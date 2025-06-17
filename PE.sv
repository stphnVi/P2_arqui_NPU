module PE(
    input logic clk, 
    input logic rst, 
    input logic signed [7:0] in_north, 
    input logic signed [7:0] in_west, 
    output logic [7:0] out_south, 
    output logic [7:0] out_east, 
    output logic signed [31:0] result,
    output logic signed [31:0] relu_out
);

    logic signed [15:0] product;
    logic signed [31:0] accum;

    assign product = in_north * in_west;
    assign accum = result + product;

    always_ff @(posedge clk or posedge rst) begin
        if (rst) begin
            result <= 32'sd0;
            out_east <= 8'sd0;
            out_south <= 8'sd0;
        end
        else begin
            result <= accum;
            out_east <= in_west;
            out_south <= in_north;
        end
    end

    assign relu_out = (result < 0) ? 32'sd0 : result;

endmodule

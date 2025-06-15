module NPU(
    input clk,
    input rst_n,

    input            in_valid,
    input [7:0]      K,
    input [7:0]      M,
    input [7:0]      N,
    output  reg      busy,

    output           A_we,
    output [15:0]    A_index,
    output [31:0]    A_din,
    input  [31:0]    A_dout,

    output           B_we,
    output [15:0]    B_index,
    output [31:0]    B_din,
    input  [31:0]    B_dout,

    output           C_we,
    output [15:0]    C_index,
    output [127:0]   C_din,
    input  [127:0]   C_dout
);


endmodule

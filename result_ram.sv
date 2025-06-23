module result_ram (
    input  logic        clk,
    input  logic        we,
    input  logic [5:0]  addr,
    input  logic [31:0] din,
    output logic [31:0] dout
);

    logic [31:0] RAM [0:63];

    always_ff @(posedge clk) begin
        if (we)
            RAM[addr] <= din;
    end

    assign dout = RAM[addr];

endmodule

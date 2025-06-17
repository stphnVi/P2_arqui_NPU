`timescale 1ns/1ps

module Systolic_Array_tb;

    logic clk;
    logic rst;
    logic [7:0] in_west [4];
    logic [7:0] in_north [4];
    logic done;
    logic [31:0] result [4][4];

    systolic_array #(.SIZE(4)) dut (
        .clk(clk),
        .rst(rst),
        .in_west(in_west),
        .in_north(in_north),
        .done(done),
        .result(result)
    );

    // Clock
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    // Stimulus
    initial begin
        rst = 1;
        #10;
        rst = 0;

        // Identidad parcial A y B
		  // valores que recibe del lado izq (filas)
        in_west[0] = 8'd1;
        in_west[1] = 8'd0;
        in_west[2] = 8'd0;
        in_west[3] = 8'd0;
		  //valores que recibe por arroba (columnas)

        in_north[0] = 8'd1;
        in_north[1] = 8'd0;
        in_north[2] = 8'd0;
        in_north[3] = 8'd0;

        wait (done);
        $display("Resultado matriz C:");
        for (int i = 0; i < 4; i++) begin
            for (int j = 0; j < 4; j++) begin
                $write("%0d\t", result[i][j]);
            end
            $display("");
        end

        $stop;
    end

endmodule

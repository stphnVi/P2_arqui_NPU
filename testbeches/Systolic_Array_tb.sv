`timescale 1ns/1ps

module Systolic_Array_tb;

    logic clk;
    logic rst;
 	 logic signed [7:0] in_west [4];
	 logic signed [7:0] in_north [4];
    logic done;
	 logic [31:0] result [4][4];        // salida con ReLU
	 logic [31:0] result_raw [4][4];    // salida sin ReLU (bruta)


	     systolic_array #(
        .SIZE(4)
    ) dut (
        .clk(clk),
        .rst(rst),
        .in_west(in_west),
        .in_north(in_north),
        .done(done),
		  .result(result),
		  .result_raw(result_raw)
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

		 // Fuerzo un valor negativo
//		 in_west[0] = -8'sd2;   // PE(0,0) va a acumular un valor negativo
//		 in_west[1] = 8'sd0;
//		 in_west[2] = 8'sd0;
//		 in_west[3] = 8'sd0;
//
//		 in_north[0] = 8'sd3;    // 3 * (-2) = -6 → ReLU = 0
//		 in_north[1] = 8'sd0;
//		 in_north[2] = 8'sd0;
//		 in_north[3] = 8'sd0;

		 in_north[0] = 8'sd1;
	 	 in_north[1] = 8'sd2;
	 	 in_north[2] = 8'sd0;
		 in_north[3] = 8'sd0;
		 
		 in_west[0] = 8'sd2;
		 in_west[1] = -8'sd1;
		 in_west[2] = 8'sd3;
		 in_west[3] = 8'sd0;
		 
		 
		 #10
		 // Borrar señales para que no sigan fluyendo
		 for (int i = 0; i < 4; i++) begin
			  in_west[i] = 8'sd0;
			  in_north[i] = 8'sd0;
		 end

		 wait (done);

		 $display("Resultado matriz C con ReLU:");
		 for (int i = 0; i < 4; i++) begin
			  for (int j = 0; j < 4; j++) begin
					$write("%0d\t", result[i][j]);
			  end
			  $display("");
		 end
		 
		 		 $display("Resultado matriz C sin ReLU:");
		 for (int i = 0; i < 4; i++) begin
			  for (int j = 0; j < 4; j++) begin
					$write("%0d\t", $signed(result_raw[i][j]));;
			  end
			  $display("");
		 end

		 $stop;
	end

endmodule

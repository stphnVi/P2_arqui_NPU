//`include "PE.sv"

//POR AHORA SOLO DE 4*4

module systolic_array #(
    parameter SIZE = 4,
    parameter DATA_WIDTH = 8,
    parameter RESULT_WIDTH = 32
)(
    input logic clk,
    input logic rst,
    input logic [DATA_WIDTH-1:0] in_west [0:SIZE-1],
    input logic [DATA_WIDTH-1:0] in_north [0:SIZE-1],
    output logic done,
    output logic [RESULT_WIDTH-1:0] result [0:SIZE-1][0:SIZE-1]
);

    // Interconexiones entre PEs
    logic [DATA_WIDTH-1:0] out_south [0:SIZE-1][0:SIZE-1];
    logic [DATA_WIDTH-1:0] out_east [0:SIZE-1][0:SIZE-1];

    // Contador de ciclos para decir cuándo termina el procesamiento
    logic [$clog2(SIZE*2):0] count;

    genvar row, col;
	 
	 //aqui se genera el arreglo systolico con los PE y su instancia de forma implicita
	 // con generate
    generate
        for (row = 0; row < SIZE; row++) begin: rows
            for (col = 0; col < SIZE; col++) begin: cols
				
				//generacion de varios PE en este caso 16 (4x4)
				
                PE pe_inst (
                    .clk(clk),
                    .rst(rst),
                    .in_north(
                        (row == 0) ? in_north[col] : out_south[row-1][col]
                    ),
                    .in_west(
                        (col == 0) ? in_west[row] : out_east[row][col-1]
                    ),
                    .out_south(out_south[row][col]),
                    .out_east(out_east[row][col]),
                    .result(result[row][col])
                );
            end
        end
    endgenerate

    // Control de finalización
    always_ff @(posedge clk or posedge rst) begin
        if (rst) begin
            count <= 0;
            done <= 0;
        end else begin
            if (count == (SIZE * 2)) begin
                done <= 1;
            end else begin
                count <= count + 1;
                done <= 0;
            end
        end
    end

endmodule

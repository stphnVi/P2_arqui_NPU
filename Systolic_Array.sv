// systolic_array.sv
// Arreglo sistólico 4x4 con activación ReLU por PE

module systolic_array #(
    parameter SIZE = 4,
    parameter DATA_WIDTH = 8,
    parameter RESULT_WIDTH = 32
)(
    input  logic clk,
    input  logic rst,
	 input logic signed [DATA_WIDTH-1:0] in_west [0:SIZE-1],
	 input logic signed [DATA_WIDTH-1:0] in_north [0:SIZE-1],
    output logic done,
    output logic [RESULT_WIDTH-1:0] result [0:SIZE-1][0:SIZE-1],
	 output logic [RESULT_WIDTH-1:0] result_raw [0:SIZE-1][0:SIZE-1]

);

    // Interconexiones entre PEs
    logic [DATA_WIDTH-1:0] out_south [0:SIZE-1][0:SIZE-1];
    logic [DATA_WIDTH-1:0] out_east  [0:SIZE-1][0:SIZE-1];
    logic [RESULT_WIDTH-1:0] relu_result [0:SIZE-1][0:SIZE-1];  // Salidas activadas por ReLU
	 logic [RESULT_WIDTH-1:0] pe_result [0:SIZE-1][0:SIZE-1];


    // Contador de ciclos para marcar cuándo termina la operación
    logic [$clog2(SIZE*2):0] count;

    genvar row, col;

    //aqui se genera el arreglo systolico con los PE y su instancia de forma implicita
	 // con generate
    generate
        for (row = 0; row < SIZE; row++) begin: rows
            for (col = 0; col < SIZE; col++) begin: cols

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
                    .result(pe_result[row][col]),  // Valor interno crudo
                    .relu_out(relu_result[row][col])  // Salida activada
                );

                // Conectar salida activada al arreglo final de resultados
                assign result[row][col]      = relu_result[row][col];
					 assign result_raw[row][col]  = pe_result[row][col];

            end
        end
    endgenerate

    // Control de finalización: espera SIZE*2 ciclos
    always_ff @(posedge clk or posedge rst) begin
        if (rst) begin
            count <= 0;
            done  <= 0;
        end else begin
            if (count == (SIZE * 2)) begin
                done <= 1;
            end else begin
                count <= count + 1;
                done  <= 0;
            end
        end
    end

endmodule

`timescale 1ns / 1ps

module PE_tb;

  // Entradas
  logic clk;
  logic rst;
  logic [7:0] in_north;
  logic [7:0] in_west;

  // Salidas
  logic [7:0] out_south;
  logic [7:0] out_east;
  logic [31:0] result;

  // Instanciar el módulo
  PE uut (
    .clk(clk),
    .rst(rst),
    .in_north(in_north),
    .in_west(in_west),
    .out_south(out_south),
    .out_east(out_east),
    .result(result)
  );

  // Clock de 10ns
  always #5 clk = ~clk;

  initial begin
    // Inicialización
    clk = 0;
    rst = 1;
    in_north = 0;
    in_west = 0;

    // Reset
    #10;
    rst = 0;

    // Primer ciclo: 2 * 3
    in_north = 8'd2;
    in_west  = 8'd3;
    #10;

    // Segundo ciclo: 4 * 5
    in_north = 8'd4;
    in_west  = 8'd5;
    #10;

    // Tercer ciclo: 1 * 6
    in_north = 8'd1;
    in_west  = 8'd6;
    #10;

    // Terminar simulación
    $display("Resultado final acumulado: %0d", result);
    $finish;
  end

endmodule

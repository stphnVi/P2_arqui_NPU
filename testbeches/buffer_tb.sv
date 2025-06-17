`timescale 1ns/1ps

module buffer_tb;

  // Parámetros del buffer
  parameter ADDR = 4;   // 4 bits de dirección → 16 posiciones
  parameter DATA = 8;   // Cada dato es de 8 bits

  // Señales
  logic clk;
  logic rst;
  logic we;
  logic [ADDR-1:0] index;
  logic [DATA-1:0] din;
  logic [DATA-1:0] dout;

  // Instancia del buffer
  buffer #(ADDR, DATA) dut (
    .clk(clk),
    .rst(rst),
    .we(we),
    .index(index),
    .din(din),
    .dout(dout)
  );

  // Clock: periodo de 10ns
  always #5 clk = ~clk;

  initial begin
    $display("Iniciando testbench de buffer");
    clk = 0;
    rst = 0;
    we = 0;
    index = 0;
    din = 0;

    // Reset
    #3; rst = 1;
    #3; rst = 0;
    #10; rst = 1; // Salimos del reset

    // -----------------------------
    // Escribir valores
    // -----------------------------
    @(negedge clk);
    we = 1;
    index = 4'd2;
    din = 8'hAA; // Valor A

    @(negedge clk);
    index = 4'd5;
    din = 8'h55; // Valor B

    @(negedge clk);
    index = 4'd10;
    din = 8'hF0; // Valor C

    // -----------------------------
    // Leer valores
    // -----------------------------
    @(negedge clk);
    we = 0;
    index = 4'd2;

    @(negedge clk);
    $display("Lectura de index 2: %h (esperado: AA)", dout);

    @(negedge clk);
    index = 4'd5;

    @(negedge clk);
    $display("Lectura de index 5: %h (esperado: 55)", dout);

    @(negedge clk);
    index = 4'd10;

    @(negedge clk);
    $display("Lectura de index 10: %h (esperado: F0)", dout);

    $finish;
  end

endmodule

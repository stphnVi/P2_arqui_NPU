module pe_with_ram_tb;

    logic clk = 0;
    logic rst;
    logic [7:0] in_north, in_west;
    logic [7:0] out_south, out_east;
    logic [31:0] result;
    
    logic we;
    logic [5:0] addr;
    logic [31:0] dout;

    // Clock
    always #5 clk = ~clk;

    // Instancia del PE
    PE dut (
        .clk(clk),
        .rst(rst),
        .in_north(in_north),
        .in_west(in_west),
        .out_south(out_south),
        .out_east(out_east),
        .result(result)
    );

    // Instancia de RAM
    result_ram ram (
        .clk(clk),
        .we(we),
        .addr(addr),
        .din(result),
        .dout(dout)
    );

    initial begin
        // Inicializa
        rst = 1;
        in_north = 0;
        in_west  = 0;
        we = 0;
        addr = 0;

        #10 rst = 0;

        // Estimulo 1
        in_north = 8'd2;
        in_west  = 8'd3;
        #10; // resultado = 6

        // Estimulo 2
        in_north = 8'd4;
        in_west  = 8'd5;
        #10; // resultado = 6 + 20 = 26

        // Guarda en RAM
        we = 1;
        addr = 6'd0;
        #10 we = 0;

        // Otro ciclo
        in_north = 8'd1;
        in_west  = 8'd1;
        #10;

        // Guarda nuevo resultado acumulado = 27
        we = 1;
        addr = 6'd1;
        #10 we = 0;

        // Exporta
        $writememh("resultados.hex", ram.RAM);

        $finish;
    end
endmodule

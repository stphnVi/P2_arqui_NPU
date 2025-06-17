module result_ram_tb;

    logic clk = 0;
    logic we;
    logic [5:0] addr;
    logic [31:0] din, dout;

    // Clock
    always #5 clk = ~clk;

    // Instancia de la RAM
    result_ram ram_inst (
        .clk(clk),
        .we(we),
        .addr(addr),
        .din(din),
        .dout(dout)
    );

    initial begin
        $display("Inicio test RAM...");
        
        // Inicializa señales
        we   = 0;
        addr = 0;
        din  = 0;

        // Ciclo 1: Escribe en posición 0
        #10;
        addr = 6'd0;
        din  = 32'hDEADBEEF;
        we   = 1;
        #10;
        we   = 0;

        // Ciclo 2: Escribe en posición 1
        addr = 6'd1;
        din  = 32'h12345678;
        we   = 1;
        #10;
        we   = 0;

        // Lectura (combinacional, se ve directo en dout)
        addr = 6'd0;
        #5 $display("RAM[0] = %h (esperado DEADBEEF)", dout);
        addr = 6'd1;
        #5 $display("RAM[1] = %h (esperado 12345678)", dout);

        // Exporta RAM a archivo
        //$writememh("C:/Users/steph/OneDrive/Documentos/GitHub/P2_arqui_NPU/resultados.hex", ram_inst.RAM);

        $finish;
    end

endmodule

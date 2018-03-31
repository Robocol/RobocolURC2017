module moduloPWM(reset, clock, inPWM, outPWM);

//INPUTS AND OUTPUTS
input reset;//Reset
input clock;//Clock
input [7:0] inPWM;
output outPWM;

//REGS
reg salidaActual=0;//Salida
assign outPWM=salidaActual;//Asignacion de la salida

//WIRES
wire flagSubida;
wire flagBajada;


//MODULOS

contador contadorSubida(reset, clock, 25000, flagSubida);
contador contadorBajada(flagSubida, clock, 100*inPWM, flagBajada);


//Logica secuencial MAQUINA DE ESTADOS
always@(posedge clock)
begin
	if(flagSubida)
		salidaActual<=1;
	else
	begin
		if(flagBajada)
			salidaActual<=0;
	end
end


endmodule

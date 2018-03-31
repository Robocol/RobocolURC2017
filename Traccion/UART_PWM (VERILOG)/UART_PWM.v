
//=======================================================
//  This code is generated by Terasic System Builder
//=======================================================

module UART_PWM(

	//////////// ADC //////////
	output		          		ADC_CONVST,
	output		          		ADC_SCK,
	output		          		ADC_SDI,
	input 		          		ADC_SDO,

	//////////// ARDUINO //////////
	inout 		    [15:0]		ARDUINO_IO,
	inout 		          		ARDUINO_RESET_N,

	//////////// CLOCK //////////
	input 		          		FPGA_CLK1_50,
	input 		          		FPGA_CLK2_50,
	input 		          		FPGA_CLK3_50,

	//////////// HPS //////////
//	inout 		          		HPS_CONV_USB_N,
//	output		    [14:0]		HPS_DDR3_ADDR,
//	output		     [2:0]		HPS_DDR3_BA,
//	output		          		HPS_DDR3_CAS_N,
//	output		          		HPS_DDR3_CK_N,
//	output		          		HPS_DDR3_CK_P,
//	output		          		HPS_DDR3_CKE,
//	output		          		HPS_DDR3_CS_N,
//	output		     [3:0]		HPS_DDR3_DM,
//	inout 		    [31:0]		HPS_DDR3_DQ,
//	inout 		     [3:0]		HPS_DDR3_DQS_N,
//	inout 		     [3:0]		HPS_DDR3_DQS_P,
//	output		          		HPS_DDR3_ODT,
//	output		          		HPS_DDR3_RAS_N,
//	output		          		HPS_DDR3_RESET_N,
//	input 		          		HPS_DDR3_RZQ,
//	output		          		HPS_DDR3_WE_N,
//	output		          		HPS_ENET_GTX_CLK,
//	inout 		          		HPS_ENET_INT_N,
//	output		          		HPS_ENET_MDC,
//	inout 		          		HPS_ENET_MDIO,
//	input 		          		HPS_ENET_RX_CLK,
//	input 		     [3:0]		HPS_ENET_RX_DATA,
//	input 		          		HPS_ENET_RX_DV,
//	output		     [3:0]		HPS_ENET_TX_DATA,
//	output		          		HPS_ENET_TX_EN,
//	inout 		          		HPS_GSENSOR_INT,
//	inout 		          		HPS_I2C0_SCLK,
//	inout 		          		HPS_I2C0_SDAT,
//	inout 		          		HPS_I2C1_SCLK,
//	inout 		          		HPS_I2C1_SDAT,
//	inout 		          		HPS_KEY,
//	inout 		          		HPS_LED,
//	inout 		          		HPS_LTC_GPIO,
//	output		          		HPS_SD_CLK,
//	inout 		          		HPS_SD_CMD,
//	inout 		     [3:0]		HPS_SD_DATA,
//	output		          		HPS_SPIM_CLK,
//	input 		          		HPS_SPIM_MISO,
//	output		          		HPS_SPIM_MOSI,
//	inout 		          		HPS_SPIM_SS,
//	input 		          		HPS_UART_RX,
//	output		          		HPS_UART_TX,
//	input 		          		HPS_USB_CLKOUT,
//	inout 		     [7:0]		HPS_USB_DATA,
//	input 		          		HPS_USB_DIR,
//	input 		          		HPS_USB_NXT,
//	output		          		HPS_USB_STP,

	//////////// KEY //////////
	input 		     [1:0]		KEY,

	//////////// LED //////////
	output		     [7:0]		LED,

	//////////// SW //////////
	input 		     [3:0]		SW,

	//////////// GPIO_0, GPIO connect to GPIO Default //////////
	inout 		    [35:0]		GPIO0GPIO,

	//////////// GPIO_1, GPIO connect to GPIO Default //////////
	inout 		    [35:0]		GPIO1GPIO
);



//=======================================================
//  REG/WIRE declarations
//=======================================================


wire rx_clear_Izquierda;
wire rx_clear_Derecha;
wire rx_ready;
wire [7:0] rx_data;

wire [7:0] PWM_IZQUIERDA;
wire [7:0] PWM_DERECHA;


//=======================================================
//  Structural coding
//=======================================================



uart uartModule(8'b11111111,//tx
	    1'b1,//tx
	    FPGA_CLK1_50,
	    GPIO0GPIO[30],//tx
	    GPIO0GPIO[31],//tx
	    GPIO0GPIO[0],
	    rx_ready,
	    rx_clear_Izquierda || rx_clear_Derecha,
	    rx_data);
		 

		 
//Conexiones

Detector_Mensajes detectorIzquierda(rx_ready, rx_clear_Izquierda, {1'b0,rx_data[7:0]}, FPGA_CLK1_50, PWM_IZQUIERDA, LED[3], 8'd76); //Detectar la L
Detector_Mensajes detectorDerecha(rx_ready, rx_clear_Derecha, {1'b0,rx_data[7:0]}, FPGA_CLK1_50, PWM_DERECHA, LED[4], 8'd82); //Detectar la R

moduloPWM moduloIzquierdo1(!KEY[0], FPGA_CLK1_50, PWM_IZQUIERDA, LED[0]);
moduloPWM moduloIzquierdo2(!KEY[0], FPGA_CLK1_50, PWM_IZQUIERDA, LED[1]);
moduloPWM moduloIzquierdo3(!KEY[0], FPGA_CLK1_50, PWM_IZQUIERDA, LED[2]);

moduloPWM moduloDerecho1(!KEY[0], FPGA_CLK1_50, PWM_DERECHA, LED[5]);
moduloPWM moduloDerecho2(!KEY[0], FPGA_CLK1_50, PWM_DERECHA, LED[6]);
moduloPWM moduloDerecho3(!KEY[0], FPGA_CLK1_50, PWM_DERECHA, LED[7]);

endmodule
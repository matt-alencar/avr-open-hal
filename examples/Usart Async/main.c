#include "./main.h"

static void blink2(usart_err_t err, uint8_t data){
	if(err > USART_ERR_NONE){
		Dio_TogglePin(DIO_ID_B, DIO_PIN_5);
	}
}

int main(){
	Dio_WritePinMode(DIO_ID_B, DIO_PIN_5, DIO_MODE_OUTPUT);
	// Dio_WritePinMode(DIO_ID_D, DIO_PIN_4, DIO_MODE_OUTPUT);
	
	usart_config_t USART_CFG;
	Usart_DefaultConfig(&USART_CFG);
	USART_CFG.BaudRate = (uint32_t)1e6;
	USART_CFG.RxSampleDiv = USART_SAMPLE_DIV8;

	// USART_CFG.BaudRate = USART_CLOCK_DIV4;
	// USART_CFG.Mode = USART_MODE_SPI_MASTER;

	// USART_CFG.Parity = USART_PARITY_ODD;
	// USART_CFG.StopBits = USART_STOP_2B;
	
	Usart_Init(USART_ID_0, &USART_CFG);

	const char *str = "Hello World!\r\n";

	// Usart_RegisterTxISR(USART_ID_0, blink);
	Usart_RegisterRxErrISR(USART_ID_0, blink2);

    sei();
	const uint8_t buff_size = strlen(str);

	uint8_t received[buff_size];
	uint8_t rx_i=0;
	uint8_t state = 0;

	memset((void*)received, 0, buff_size);

	while(1){
		switch(state){
			case 0:
				Usart_WriteStr(USART_ID_0, str);
				// Usart_WriteByte(USART_ID_0, 'A');
				// Usart_WriteByte(USART_ID_0, 'a');
				// Usart_WriteByte(USART_ID_0, '\n');
				// Usart_WriteByte(USART_ID_0, '\r');
				
				// Usart_WriteWord(USART_ID_0, 0x4161);
				// Usart_WriteWord(USART_ID_0, 0x0A0D);
				// Usart_WriteDWord(USART_ID_0, 0xAF074649);
				Usart_Flush(USART_ID_0);
				state = 1;
			break;

			case 1:
				if(Usart_Available(USART_ID_0)){
					received[rx_i++] = Usart_ReadByte(USART_ID_0);
					if(rx_i >= buff_size){
						rx_i = 0;
						state = 2;
					}
				}
			break;

			case 2:
				if(Usart_State(USART_ID_0) == USART_STATE_IDLE){
					Usart_Transmit(USART_ID_0, (uint8_t*)received, buff_size);
					state = 3;
				}
			break;

			case 3:
				if(Usart_State(USART_ID_0) == USART_STATE_IDLE){
					memset((void*)received, 0, buff_size);
					state = 1;
					// _delay_ms(100);
				}
			break;
		}
	}
}
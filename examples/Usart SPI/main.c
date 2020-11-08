#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "dio.h"
#include "usart.h"
#include <string.h>


int main(){
	Dio_WritePinMode(DIO_ID_B, (DIO_PIN_5 | DIO_PIN_2), DIO_MODE_OUTPUT);
	Dio_SetPin(DIO_ID_B, DIO_PIN_2);
	// Dio_WritePinMode(DIO_ID_D, DIO_PIN_4, DIO_MODE_OUTPUT);
	
	usart_config_t USART_CFG;
	Usart_DefaultConfig(&USART_CFG);
	USART_CFG.BaudRate = USART_CLOCK_DIV2;
	USART_CFG.Mode = USART_MODE_SPI_MASTER;
	
	Usart_Init(USART_ID_0, &USART_CFG);

	const char *str = "Hello World!\r\n";

    sei();
	const uint8_t buff_size = strlen(str);

	uint8_t received[buff_size];
	uint8_t rx_i=0;

	memset((void*)received, 0, buff_size);

	Dio_SetPin(DIO_ID_B, DIO_PIN_5); // Start test

	Dio_ResetPin(DIO_ID_B, DIO_PIN_2); // Set CS pin to low
	Usart_WriteStr(USART_ID_0, str);
	// Usart_WriteByte(USART_ID_0, 'A');
	// Usart_WriteByte(USART_ID_0, 'a');
	// Usart_WriteByte(USART_ID_0, '\n');
	// Usart_WriteByte(USART_ID_0, '\r');
	// Usart_WriteWord(USART_ID_0, 0x4161);
	// Usart_WriteWord(USART_ID_0, 0x0A0D);
	// Usart_WriteDWord(USART_ID_0, 0xAF074649);
	Usart_Flush(USART_ID_0);
	while(Usart_State(USART_ID_0) != USART_STATE_IDLE);
	Dio_SetPin(DIO_ID_B, DIO_PIN_2); // Set CS pin to high

	while(Usart_Available(USART_ID_0)){
		received[rx_i++] = Usart_ReadByte(USART_ID_0);
		if(rx_i >= buff_size){
			rx_i = 0;
			if(memcmp(received, str, buff_size) == 0)
				Dio_ResetPin(DIO_ID_B, DIO_PIN_5); // Test success
			break;
		}
	}

	while(1){
		
	}
}
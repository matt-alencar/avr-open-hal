#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "dio.h"
#include "spi.h"
#include "wdt.h"

void Spi_SlaveHandler(uint8_t data);
void Spi_TimeOut();
int main();
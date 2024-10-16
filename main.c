#include "ch552.h"
#include <stdint.h>

void delay_ms(uint16_t u16Delay);

void delay_ms(uint16_t u16Delay)
{
	
	while (u16Delay) {
		TF0 = 0;
		TH0 = 0xA2;
		TL0 = 0x40;
		TR0 = 1;
		while (!TF0) {
		}
		TR0 = 0;
		--u16Delay;
	}
}

SBIT(P3_1,0xB0,1);

__xdata uint8_t u8Buff[64];

void send(uint8_t u8Data);

void send(uint8_t u8Data)
{
	uint8_t i;
	
	for (i = 0; i < 8; ++i) {
		if (u8Data & 0x80) {
			P3_1 = 1;
			delay_ms(4);
			P3_1 = 0;
			delay_ms(1);
		} else {
			P3_1 = 1;
			delay_ms(1);
			P3_1 = 0;
			delay_ms(4);
		}
		u8Data <<= 1;
	}
}

void main(void)
{
	uint8_t i;
	
	/* clock */
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	CLOCK_CFG = 0x86;
	SAFE_MOD = 0x00;
	
	/* P3.1 */
	/* Push-pull */
	P3_MOD_OC &= ~(1 << 1);
	P3_DIR_PU |= (1 << 1);
	
	P3 &= ~(1 << 1);
	
	T2MOD |= (1 << 7);
	T2MOD |= (1 << 4);
	TMOD = 0x01;
	
	USB_CTRL = 0b01101001; // Low speed
	UDEV_CTRL |= (1 << 2) | (1 << 0);
	
	while (1) {
		if (UIF_BUS_RST) {
			P3_1 = 1;
			P3_1 = 0;
			UEP0_DMA = (uint16_t)u8Buff;
			UEP0_CTRL = 0x02;
			UIF_BUS_RST = 0;
		}
		if (UIF_TRANSFER) {
			P3_1 = 1;
			P3_1 = 0;
			P3_1 = 1;
			P3_1 = 0;
			u8Buff[0] = 0x14;
			u8Buff[1] = 0x12;
			u8Buff[2] = 0x19;
			u8Buff[3] = 0x86;
			UEP0_T_LEN = 4;
			UEP0_CTRL = 0x00;
			UIF_TRANSFER = 0;
		}
	}
}

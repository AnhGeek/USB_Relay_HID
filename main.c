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

uint8_t u8UsbIndex;
const uint8_t u8DeviceDescriptor[] = {
	0x12, /* 0 */
	0x01, /* 1 */
	0x00, /* 2 */
	0x02, /* 3 */
	0xff, /* 4 */
	0x00, /* 5 */
	0x00, /* 6 */
	0x40, /* 7 */
	0x34, /* 8 */
	0x12, /* 9 */
	0x78, /* 10 */
	0x56, /* 11 */
	0x00, /* 12 */
	0x02, /* 13 */
	0x00, /* 14 */
	0x00, /* 15 */
	0x00, /* 16 */
	0x01 /* 17 */
};

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
	
	if(0){
		USB_CTRL = 0b01101001; // Low speed
		UDEV_CTRL |= (1 << 2) | (1 << 0);
	} else {
		// Highspeed
		USB_CTRL = (1 << 5) | (1 << 3) | (1 << 0);
		UDEV_CTRL |= (1 << 0); 
	}
	u8UsbIndex = 0;
	while (1) 
	{
		if (UIF_BUS_RST) 
		{
			P3_1 = 1;
			P3_1 = 0;
			UEP0_DMA = (uint16_t)u8Buff;
			UEP0_CTRL = 0x02;
			UIF_BUS_RST = 0;
		}
		
		if (UIF_TRANSFER) {
			/* Program will fall in to this handler after a transfer being successful
			   You must prepare data within the previous transaction. */
			if ((USB_INT_ST & 0x0F) == 0x00) {
				/* reset buffer */
				UEP0_T_LEN = 0;
				// New request to EP0?
				switch((USB_INT_ST & 0x30)){
					case 0x30:
						//Setup transfer
						P3_1 = 1;
						P3_1 = 0;
						if (u8Buff[0] & 0x80) {
							/* Check bmRequestType = device to host */
							
							switch (u8Buff[1]) {
								case 0x06:
									/* bRequest == Get descriptor */
									switch (u8Buff[3]) {
										//Check descriptor types
										case 0x01:
											P3_1 = 1;
											P3_1 = 0;
											P3_1 = 1;
											P3_1 = 0;
											P3_1 = 1;
											P3_1 = 0;
											/* device descriptor */
											if (u8Buff[6] >= 0x12) {
												for (i = 0; i < 0x12; ++i) {
													u8Buff[i] = u8DeviceDescriptor[i];
												}
												UEP0_T_LEN = 0x12;
												UEP0_CTRL = 0x80 | 0x40;
											}
											break;
									}
									break;
								default:
									//Default
									break;
							}
						} 
						else 
						{
							/* Check bmRequestType = host to device */
							P3_1 = 1;
							P3_1 = 0;
							P3_1 = 1;
							P3_1 = 0;
							P3_1 = 1;
							P3_1 = 0;
							P3_1 = 1;
							P3_1 = 0;
							P3_1 = 1;
							P3_1 = 0;
							switch (u8Buff[1]) {
								case 0x05:
									/* bRequest == SET_ADDRESS */
									u8UsbIndex = u8Buff[2];
									UEP0_T_LEN = 0;
									UEP0_CTRL = 0x80 | 0x40;
									break;
								default:
									//Default
									break;
							}
						}
						break;	
					case 0x20:
						//In packets
						P3_1 = 1;
						P3_1 = 0;
						P3_1 = 1;
						P3_1 = 0;
						P3_1 = 1;
						P3_1 = 0;
						P3_1 = 1;
						P3_1 = 0;
						if ((u8UsbIndex != 0x00) && (USB_DEV_AD != u8UsbIndex )) 
						{
							USB_DEV_AD = u8UsbIndex;
						}
						break;
						
					case 0x00:
						//Out packets
						P3_1 = 1;
						P3_1 = 0;
						P3_1 = 1;
						P3_1 = 0;
						P3_1 = 1;
						P3_1 = 0;
						P3_1 = 1;
						P3_1 = 0;
						P3_1 = 1;
						P3_1 = 0;
						P3_1 = 1;
						P3_1 = 0;
						break;
						
					default:
						//SOF packets
						break;
				}
			} 
			UIF_TRANSFER = 0;
		}
	}
}

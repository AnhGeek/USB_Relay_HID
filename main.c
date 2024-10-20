#include "ch552.h"
#include <stdint.h>

#define DATA_STATE 0
#define STATUS_STATE 1

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
__xdata uint8_t u8Ep1Buff[64];
__xdata uint8_t u8Ep2Buff[64];

uint8_t u8UsbIndex;
const uint8_t u8DeviceDescriptor[] = {
	0x12, /* 0 */
	0x01, /* 1 */
	0x00, /* 2 */
	0x02, /* 3 */
	0x00, /* 4 */
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

const uint8_t u8ConfigDescriptor[] = {
	0x09,
	0x02,
	0x20,
	0x00,
	0x01,
	0x01,
	0x00,
	0xC0,
	0x32,
	/* interface */
	0x09,
	0x04,
	0x00,
	0x00,
	0x02,
	0xFF,
	0xFF,
	0xFF,
	0x00,
	/* endpoint out */
	0x07,
	0x05,
	0x01,
	0x02,
	0x40,
	0x00,
	0x01,
	/* endpoint in */
	0x07,
	0x05,
	0x82,
	0x02,
	0x40,
	0x00,
	0x01,
};

void send(uint8_t u8Data);

void send(uint8_t u8Data)
{
	uint8_t i;
	
	for (i = 0; i < 8; ++i) {
		if (u8Data & 0x80) {
			//P3_1 = 1;
			delay_ms(4);
			//P3_1 = 0;
			delay_ms(1);
		} else {
			//P3_1 = 1;
			delay_ms(1);
			//P3_1 = 0;
			delay_ms(4);
		}
		u8Data <<= 1;
	}
}

void main(void)
{
	uint8_t i;
	uint8_t tmp;
	uint8_t u8ControlState = DATA_STATE;
	
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
			//P3_1 = 1;
			//P3_1 = 0;
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
						//P3_1 = 1;
						//P3_1 = 0;
						if (u8Buff[0] & 0x80) {
							/* Check bmRequestType = device to host */
							
							switch (u8Buff[1]) {
								case 0x06:
									/* bRequest == Get descriptor */
									switch (u8Buff[3]) {
										//Check descriptor types
										case 0x01:
											//P3_1 = 1;
											//P3_1 = 0;
											//P3_1 = 1;
											//P3_1 = 0;
											//P3_1 = 1;
											//P3_1 = 0;
											/* device descriptor */
											u8ControlState = DATA_STATE;
											if (u8Buff[6] >= 0x12) {
												for (i = 0; i < 0x12; ++i) {
													u8Buff[i] = u8DeviceDescriptor[i];
												}
												UEP0_T_LEN = 0x12;
												UEP0_CTRL = 0x80 | 0x40;
											}
											break;
										case 0x02:
											//P3_1 = 1;
											//P3_1 = 0;
											//P3_1 = 1;
											//P3_1 = 0;
											//P3_1 = 1;
											//P3_1 = 0;
											/* config descriptor */
											u8ControlState = DATA_STATE;
											if (u8Buff[6] >= 32) {
												for (i = 0; i < 32; ++i) {
													u8Buff[i] = u8ConfigDescriptor[i];
												}
												UEP0_T_LEN = 32;
												UEP0_CTRL = 0x80 | 0x40;
											} else {
												tmp = u8Buff[6];
												for (i = 0; i < tmp; ++i) {
													u8Buff[i] = u8ConfigDescriptor[i];
												}
												UEP0_T_LEN = tmp;
												UEP0_CTRL = 0x80 | 0x40;
											}
											break;
										case 0x06:
											u8ControlState = DATA_STATE;
											UEP0_T_LEN = 0x00;
											UEP0_CTRL = 0x80 | 0x40;
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
							//P3_1 = 1;
							//P3_1 = 0;
							//P3_1 = 1;
							//P3_1 = 0;
							//P3_1 = 1;
							//P3_1 = 0;
							//P3_1 = 1;
							//P3_1 = 0;
							//P3_1 = 1;
							//P3_1 = 0;
							switch (u8Buff[1]) {
								case 0x05:
									/* bRequest == SET_ADDRESS */
									u8UsbIndex = u8Buff[2];
									UEP0_T_LEN = 0;
									UEP0_CTRL = 0x80 | 0x40;
									break;
								case 0x09:
									/* bRequest == SET_CONFIG */
									UEP1_CTRL = (1 << 4) | (1 << 1);
									UEP4_1_MOD |= (1 << 7);
									UEP4_1_MOD &= ~(1 << 6);
									UEP1_DMA = (uint16_t)u8Ep1Buff;
									
									UEP2_CTRL = (1 << 4) | (1 << 3) | (1 << 1);
									UEP2_3_MOD |= (1 << 2);
									UEP2_3_MOD &= ~(1 << 3);
									UEP2_DMA = (uint16_t)u8Ep2Buff;
									u8ControlState = STATUS_STATE;
									/* set config */
									UEP0_T_LEN = 0;
									UEP0_CTRL = 0x40;
									break;
								default:
									//Default
									break;
							}
						}
						break;	
					case 0x20:
						/* EP0 in */
						//P3_1 = 1;
						//P3_1 = 0;
						//P3_1 = 1;
						//P3_1 = 0;
						//P3_1 = 1;
						//P3_1 = 0;
						//P3_1 = 1;
						//P3_1 = 0;
						if (u8ControlState == DATA_STATE) {
							u8ControlState = STATUS_STATE;
							UEP0_CTRL = 0x80 | 0x40 | 0x02;
						} else {
							/* EP0 in */
							if ((u8UsbIndex != 0x00) && (USB_DEV_AD != u8UsbIndex )) 
							{
								USB_DEV_AD = u8UsbIndex;
							}
							UEP0_CTRL = 0x40 | 0x02;
						}
						break;
						
					case 0x00:
						/* EP0 out */
						//P3_1 = 1;
						//P3_1 = 0;
						//P3_1 = 1;
						//P3_1 = 0;
						//P3_1 = 1;
						//P3_1 = 0;
						//P3_1 = 1;
						//P3_1 = 0;
						//P3_1 = 1;
						//P3_1 = 0;
						//P3_1 = 1;
						//P3_1 = 0;
						break;
						
					default:
						//SOF packets
						break;
				}
			} else if ((USB_INT_ST & 0x0F) == 0x01) 
			{
				/* ep1 */
				if (u8Ep1Buff[0] == 1) 
				{
					P3_1 = 1;
				} else if (u8Ep1Buff[0] == 2) 
				{
					P3_1 = 0;
				} else if (u8Ep1Buff[0] == 3) 
				{
					P3_1 = 1;
					P3_1 = 0;
					u8Ep2Buff[0] = 0x05;
					u8Ep2Buff[1] = 0x10;
					u8Ep2Buff[2] = 0x19;
					u8Ep2Buff[3] = 0x94;
					UEP2_T_LEN = 0x40;
					tmp = UEP2_CTRL;
					tmp &= ~(1 << 1);
					tmp &= ~(1 << 0);
					UEP2_CTRL = tmp;
				}
			} else if ((USB_INT_ST & 0x0F) == 0x02) 
			{
				/* ep2 */
				tmp = UEP2_CTRL;
				tmp |= (1 << 1);
				tmp &= ~(1 << 0);
				UEP2_CTRL = tmp;
			}
			UIF_TRANSFER = 0;
		}
	}
}

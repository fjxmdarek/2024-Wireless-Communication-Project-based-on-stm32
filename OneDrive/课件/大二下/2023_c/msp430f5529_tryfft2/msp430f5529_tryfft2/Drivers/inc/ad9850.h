#ifndef _AD9850_H
#define _AD9850_H
#define PIN_SET 1
#define PIN_RESET 0
#include "msp430f5529_gpio.h"
//#include "main.h"  //改成自己库头文件
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long int u32;

#define AD9850_CONTROL_PORT  P2
#define AD9850_FQUD   4  //改成自己连接的引脚
#define AD9850_WCLK   5  //改成自己连接的引脚
#define AD9850_DATA  3  //改成自己连接的引脚
#define AD9850_RST   2  //改成自己连接的引脚

#define AD9850_WCLK_H	PIN_OUT(AD9850_CONTROL_PORT,AD9850_WCLK)=1
#define AD9850_WCLK_L	PIN_OUT(AD9850_CONTROL_PORT,AD9850_WCLK)=0
#define AD9850_FQUD_H	PIN_OUT(AD9850_CONTROL_PORT,AD9850_FQUD)=1
#define AD9850_FQUD_L	PIN_OUT(AD9850_CONTROL_PORT,AD9850_FQUD)=0
#define AD9850_DATA_H	PIN_OUT(AD9850_CONTROL_PORT,AD9850_DATA)=1
#define AD9850_DATA_L	PIN_OUT(AD9850_CONTROL_PORT,AD9850_DATA)=0
#define AD9850_RESET_H	PIN_OUT(AD9850_CONTROL_PORT,AD9850_RST)=1
#define AD9850_RESET_L	PIN_OUT(AD9850_CONTROL_PORT,AD9850_RST)=0


void AD9850_Delay(unsigned int z); 
void AD9850_Write(unsigned char w0,double frequence);
//void AD9850_Write(u8 w0,double frequence);
void AD9850_Init(void);

#endif


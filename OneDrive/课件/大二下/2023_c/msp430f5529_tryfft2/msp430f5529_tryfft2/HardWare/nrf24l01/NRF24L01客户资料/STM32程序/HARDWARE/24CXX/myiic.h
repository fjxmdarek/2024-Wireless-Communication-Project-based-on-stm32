#ifndef __MYIIC_H
#define __MYIIC_H
#include "common.h"


typedef enum 
{                         //端口              SCL          SDA
  I2C0          =0,      //I2C_B0            P3.1         P3.0
  I2C1          =1,      //I2C_B1            P4.2         P4.1
}I2Cn;         //I2Cn模块


uint8_t  READ_SDA(I2Cn i2cn);	   		   
//IO方向设置
void  SDA_TYPE(I2Cn i2cn,uint8_t levl);


//IO操作函数	 
void IIC_SCL(I2Cn i2cn,uint8_t levl);
void IIC_SDA(I2Cn i2cn,uint8_t levl);


//IIC所有操作函数
void IIC_Init(I2Cn i2cn,uint16_t SlaveID);                //初始化IIC的IO口				 
void IIC_Start(I2Cn i2cn);				//发送IIC开始信号
void IIC_Stop(I2Cn i2cn);	  			//发送IIC停止信号
void IIC_Send_Byte(I2Cn i2cn,uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(I2Cn i2cn,unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(I2Cn i2cn); 				//IIC等待ACK信号
void IIC_Ack(I2Cn i2cn);					//IIC发送ACK信号
void IIC_NAck(I2Cn i2cn);				//IIC不发送ACK信号

void IIC_Write_One_Byte(I2Cn i2cn,uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(I2Cn i2cn,uint8_t daddr,uint8_t addr);	  
#endif

















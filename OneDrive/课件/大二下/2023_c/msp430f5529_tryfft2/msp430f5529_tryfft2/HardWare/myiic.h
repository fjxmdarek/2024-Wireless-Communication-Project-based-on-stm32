#ifndef __MYIIC_H
#define __MYIIC_H
#include "common.h"


typedef enum 
{                         //端口              SCL          SDA
  I2C_0          =0,      //I2C_B0            P3.1         P3.0
  I2C_1          =1,      //I2C_B1            P4.2         P4.1
}I2C_n;         //I2C_n模块
//****************************************
// 定义MPU6050内部地址
//****************************************
#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I			0x68//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	0xD0	//IIC写入时的地址字节数据，+1为读取
//****************************************


void InitMPU6050();                 //初始化MPU6050

uint8_t  READ_SDA(I2C_n i2cn);	   		   
//IO方向设置
void  SDA_TYPE(I2C_n i2cn,uint8_t levl);


//IO操作函数	 
void IIC_SCL(I2C_n i2cn,uint8_t levl);
void IIC_SDA(I2C_n i2cn,uint8_t levl);


//IIC所有操作函数
void IIC_Init(I2C_n i2cn,uint16_t SlaveID);                //初始化IIC的IO口				 
void IIC_Start(I2C_n i2cn);				//发送IIC开始信号
void IIC_Stop(I2C_n i2cn);	  			//发送IIC停止信号
void IIC_Send_Byte(I2C_n i2cn,uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(I2C_n i2cn,unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(I2C_n i2cn); 				//IIC等待ACK信号
void IIC_Ack(I2C_n i2cn);					//IIC发送ACK信号
void IIC_NAck(I2C_n i2cn);				//IIC不发送ACK信号

void IIC_Write_One_Byte(I2C_n i2cn,uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(I2C_n i2cn,uint8_t daddr,uint8_t addr);	  
#endif

















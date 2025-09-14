#include "myiic.h"
#include "msp430f5529_gpio.h"
#include "msp430f5529_system.h"

const struct{
  GPIO_PIN SCL;
  GPIO_PIN SDA;
}MY_I2C_PIN[]=
{ //        SCL         SDA
  /*I2C_0*/{.SCL={P3,1},.SDA={P3,0}},           
  /*I2C1*/{.SCL={P4,2},.SDA={P4,1}}          
};
uint8_t  READ_SDA(I2C_n i2cn)
{
  return GPIO_ReadBit(MY_I2C_PIN[i2cn-I2C_0].SDA.Port,MY_I2C_PIN[i2cn-I2C_0].SDA.Pin);  
} 
void  SDA_TYPE(I2C_n i2cn,uint8_t levl)
{
  if(levl==1)
  {
    GPIO_Init(MY_I2C_PIN[i2cn-I2C_0].SDA.Port,MY_I2C_PIN[i2cn-I2C_0].SDA.Pin,GPO);
  }
  else
  {
    GPIO_Init(MY_I2C_PIN[i2cn-I2C_0].SDA.Port,MY_I2C_PIN[i2cn-I2C_0].SDA.Pin,GPI);
    GPIO_Init(MY_I2C_PIN[i2cn-I2C_0].SDA.Port,MY_I2C_PIN[i2cn-I2C_0].SDA.Pin,PULL_UP);
    
  }
}
void IIC_SCL(I2C_n i2cn,uint8_t levl)    
{
  if(levl==1)
    GPIO_WriteBit  (MY_I2C_PIN[i2cn-I2C_0].SCL.Port,MY_I2C_PIN[i2cn-I2C_0].SCL.Pin,BIT_SET); //SCL
  else
    GPIO_WriteBit  (MY_I2C_PIN[i2cn-I2C_0].SCL.Port,MY_I2C_PIN[i2cn-I2C_0].SCL.Pin,RESET); //SCL
}
void IIC_SDA(I2C_n i2cn,uint8_t levl)    
{
  if(levl==1)
    GPIO_WriteBit  (MY_I2C_PIN[i2cn-I2C_0].SDA.Port,MY_I2C_PIN[i2cn-I2C_0].SDA.Pin,BIT_SET); //SCL
  else
    GPIO_WriteBit  (MY_I2C_PIN[i2cn-I2C_0].SDA.Port,MY_I2C_PIN[i2cn-I2C_0].SDA.Pin,RESET); //SCL
}
/*******************************************************************************
*  函数名称：IIC_Init(I2C_n i2cn,uint16_t SlaveID) 
*  功能说明：初始化一个模块为I2C主机模块，并设置从机地址
*  参数说明：I2C_n i2cn      :模块号
             uint8_t SlaveID  :从机地址
             
*  函数返回：无
*  使用示例：I2C_MasterInit   (I2C_0, 0x68);  //初始化I2C_0模块为主机，从机地址为0x68
********************************************************************************/
//初始化IIC

void IIC_Init(I2C_n i2cn,uint16_t SlaveID)
{	//输出9个时钟以恢复I2C_n总线状态  
        for(uint8_t i=0;i<9;i++)
        {
          GPIO_WriteBit  (MY_I2C_PIN[i2cn-I2C_0].SCL.Port,MY_I2C_PIN[i2cn-I2C_0].SCL.Pin,  BIT_SET);
          DELAY_US(5);
          GPIO_WriteBit  (MY_I2C_PIN[i2cn-I2C_0].SCL.Port,MY_I2C_PIN[i2cn-I2C_0].SCL.Pin,RESET);
          DELAY_US(5);
        }
        //初始化引脚
        GPIO_Init(MY_I2C_PIN[i2cn-I2C_0].SCL.Port,MY_I2C_PIN[i2cn-I2C_0].SCL.Pin,GPO);
        GPIO_Init(MY_I2C_PIN[i2cn-I2C_0].SDA.Port,MY_I2C_PIN[i2cn-I2C_0].SDA.Pin,GPO);
        
        GPIO_WriteBit  (MY_I2C_PIN[i2cn-I2C_0].SCL.Port,MY_I2C_PIN[i2cn-I2C_0].SCL.Pin,BIT_SET);
        GPIO_WriteBit  (MY_I2C_PIN[i2cn-I2C_0].SDA.Port,MY_I2C_PIN[i2cn-I2C_0].SDA.Pin,BIT_SET);
        
        GPIO_WriteBit  (MY_I2C_PIN[i2cn-I2C_0].SCL.Port,MY_I2C_PIN[i2cn-I2C_0].SCL.Pin,PULL_UP);
        GPIO_WriteBit  (MY_I2C_PIN[i2cn-I2C_0].SDA.Port,MY_I2C_PIN[i2cn-I2C_0].SDA.Pin,PULL_UP);
 	
}
//产生IIC起始信号
void IIC_Start(I2C_n i2cn)
{
	SDA_TYPE(i2cn,1);     //sda线输出
	IIC_SDA(i2cn,1);	  	  
	IIC_SCL(i2cn,1);
	DELAY_US(4);
 	IIC_SDA(i2cn,0);//START:when CLK is high,DATA change form high to low 
	DELAY_US(4);
	IIC_SCL(i2cn,0);//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(I2C_n i2cn)
{
	SDA_TYPE(i2cn,1);//sda线输出
        IIC_SDA(i2cn,0);//STOP:when CLK is high DATA change form low to high
	IIC_SCL(i2cn,1);
	
 	DELAY_US(4);
	IIC_SCL(i2cn,1); 
	IIC_SDA(i2cn,1);//发送I2C总线结束信号
	DELAY_US(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(I2C_n i2cn)
{
	uint8_t ucErrTime=0;
	SDA_TYPE(i2cn,0);      //SDA设置为输入  
	IIC_SDA(i2cn,1);DELAY_US(1);	   
	IIC_SCL(i2cn,1);DELAY_US(1);	 
	while(READ_SDA(i2cn))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop(i2cn);
			return 1;
		}
	}
	IIC_SCL(i2cn,0);//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(I2C_n i2cn)
{
	IIC_SCL(i2cn,0);
	SDA_TYPE(i2cn,1);
	IIC_SDA(i2cn,0);
	DELAY_US(2);
	IIC_SCL(i2cn,1);
	DELAY_US(2);
	IIC_SCL(i2cn,0);
}
//不产生ACK应答		    
void IIC_NAck(I2C_n i2cn)
{
	IIC_SCL(i2cn,0);
	SDA_TYPE(i2cn,1);
	IIC_SDA(i2cn,1);
	DELAY_US(2);
	IIC_SCL(i2cn,1);
	DELAY_US(2);
	IIC_SCL(i2cn,0);
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(I2C_n i2cn,uint8_t txd)
{                        
    uint8_t t;   
	SDA_TYPE(i2cn,1); 	    
    IIC_SCL(i2cn,0);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA(i2cn,(txd&0x80)>>7);
        txd<<=1; 	  
		DELAY_US(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL(i2cn,1);
		DELAY_US(2); 
		IIC_SCL(i2cn,0);	
		DELAY_US(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(I2C_n i2cn,unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_TYPE(i2cn,0);//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL(i2cn,0); 
        DELAY_US(2);
		IIC_SCL(i2cn,1);
        receive<<=1;
        if(READ_SDA(i2cn))receive++;   
		DELAY_US(1); 
    }					 
    if (!ack)
        IIC_NAck(i2cn);//发送nACK
    else
        IIC_Ack(i2cn); //发送ACK   
    return receive;
}
void IIC_Write_One_Byte(I2C_n i2cn,uint8_t daddr,uint8_t addr,uint8_t data)
{
    IIC_Start(i2cn);  
    IIC_Send_Byte(i2cn,daddr<<1);   //发送器件地址0XA0,写数据 	 
    IIC_Wait_Ack(i2cn);
    IIC_Send_Byte(i2cn,addr);   //发送地址
    IIC_Wait_Ack(i2cn); 	 										  		   
    IIC_Send_Byte(i2cn,data);     //发送字节							   
    IIC_Wait_Ack(i2cn);  		    	   
    IIC_Stop(i2cn);//产生一个停止条件 
	DELAY_US(100);
  
}
uint8_t IIC_Read_One_Byte(I2C_n i2cn,uint8_t daddr,uint8_t addr)
{
    uint8_t temp=0;		  	    																 
    IIC_Start(i2cn);  
    IIC_Send_Byte(i2cn,daddr<<1);   //发送器件地址0XA0,写数据 	 
    IIC_Wait_Ack(i2cn);
    IIC_Send_Byte(i2cn,addr);   //发送地址
    IIC_Wait_Ack(i2cn);	    
    IIC_Start(i2cn);  	 	   
    IIC_Send_Byte(i2cn,(daddr<<1)|0x01);           //进入接收模式			   
    IIC_Wait_Ack(i2cn);	 
    temp=IIC_Read_Byte(i2cn,0);		   
    IIC_Stop(i2cn);//产生一个停止条件	    
	return temp;
  
}



























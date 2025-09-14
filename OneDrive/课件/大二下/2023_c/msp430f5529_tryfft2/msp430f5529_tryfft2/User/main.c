/*        
头文件声明        
*/
#include "msp430f5529_system.h"
#include "msp430f5529_clock.h"
#include "msp430f5529_watchdog.h"
#include "msp430f5529_gpio.h"
#include "msp430f5529_timer.h"
#include "msp430f5529_adc.h"
#include "msp430f5529_uart.h"
#include "msp430f5529_i2c.h"
#include "msp430f5529_spi.h"
#include "msp430f5529_flash.h"
#include "msp430f5529_rtc.h"
#include "msp430f5529_dma.h"
#include "msp430f5529_compater.h"
#include "msp430f5529_lpm.h"
#include "msp430f5529_nmi.h"
#include "msp430f5529_remap.h"
#include "msp430f5529_it.h"
#include "msp430f5529_vectors.h"
#include "KEY.h"
#include "LED.h"
#include "LCD_API.h"
#include "OLED.h"
#include "math.h"
#include "PID.h"
#include "delay.h"
#include "fft.h"
#include "ad9850.h"
/*        
定义变量常量        
*/
#define Vx 225//输入电压
#define Rr 198.8//标准电阻
#define PI 3.141592653589793238
#define  Fre1 40000
#define Fre2 500000
#define Vx2 50625

int ad1[100];
int ad3[100];
int max1;
int min1;
int max2;
int min2;
u32 fre=Fre2; //设定频率
int flag_LC=1;//测量电感还是电容
int flag=1;//暂停功能标志
float Phase;//相位
float V_Phase;//相位端电压
float Vr;//电阻端电压
float VrdVx;
float R_Value;
float X_Value;
float C_Value;
float L_Value;
float D_Value;
float Q_Value;
int count=0;
float buffer_V=1000;
float buffer_P=0;
double Z_Value;
double Sin_Z;
double Vz;
double Vx_Value;
double Vr_Value;

__interrupt void TIMER1_A0_IRQ_Handler()
{
  KEY_20msTimingProcess(KEY1);
  //KEY_20msTimingProcess(KEY2);
}
void KEY1_EventHandler(KEY_EVENT event)
{
  switch(event)
  {
  case KEY_CLICK:
    flag=0;
    //LED_SetStatus(LED1, TRUE);                                        //设置LED状态
    //LED_SetStatus(LED2, FALSE);                                        //设置LED状态
  break;
  case KEY_DOUBLE_CLICK:
    flag_LC = !flag_LC;
   break;
  case KEY_LONG_CLICK:
    //LED_SetStatus(LED1, TRUE);                                        //设置LED状态
    //LED_SetStatus(LED2, TRUE);                                        //设置LED状态
    break;
  default:
    break;
  }
}
void main()
{
  DisableInterrupts();            //禁止总中断
  OLED_Init();

  ADC_Init(ADC_CH1+ADC_CH3,ADC_VREF_3_3V,ADC_10bit);       //初始化通道,P6.1,P6.3
  DMA_Init(DMA_CH1,ADC_GetChanelMemoryAddr(ADC_CH1), &ad1,DMATSEL_ADC12IFG,100, DMA_SRCAddr_KeepOn|DMA_DSTAddr_Increase|DMA_WORD|DMA_Repeated_Single|DMA_Edge);
  //DMA_ITConfig (DMA_CH1,ENABLE); 
  DMA_Init(DMA_CH2,ADC_GetChanelMemoryAddr(ADC_CH3), &ad3,DMATSEL_ADC12IFG,100, DMA_SRCAddr_KeepOn|DMA_DSTAddr_Increase|DMA_WORD|DMA_Repeated_Single|DMA_Edge);
  //DMA_ITConfig (DMA_CH2,ENABLE);
  
  
  //DMA_SoftwareTrigger(DMA_CH1);
  //for(int i = 0;i<100;i++)
  //{
    //printf("%d %f\n",i,ad1[i]*3.3/1023);
  //}
  GPIO_MultiBits_Init(P2,BIT5,GPO|HDS);   
  GPIO_MultiBits_Init(P2,BIT4,GPO|HDS);    
  GPIO_MultiBits_Init(P2,BIT3,GPO|HDS);  
  GPIO_MultiBits_Init(P2,BIT2,GPO|HDS); 
  LED_Init(LED_ALL); 
  KEY_Init(KEY1,KEY1_EventHandler); 
  Set_Vector_Handler(VECTOR_TIMER_A1_CCR0, TIMER1_A0_IRQ_Handler);   //设置定时器TA1的中断服务程序为：TIMER1_A0_IRQ_Handler
  TIMER_Interval_Ms(TIMER_A1, 20);    //设置TA1为定时中断，中断时间为500Ms
  TIMER_ITConfig (TIMER_A1, TIMER_CCR0_IRQn, ENABLE);            //使能TIMER的某一个中断
  AD9850_Init();
  
  EnableInterrupts();
  while(1)
  {  
    if(flag_LC)
    {
      fre=Fre1;
      AD9850_Write(0x00,(double)fre);
      //DELAY_MS(1);
    }
    else
    {
      fre=Fre2;
      AD9850_Write(0x00,(double)fre);
      //DELAY_MS(1);
    }
    
    ADC_StartConv();
    //DMA_SoftwareTrigger(DMA_CH1);
    while(!DMA_GetITStatus(DMA_CH1)&&!DMA_GetITStatus(DMA_CH2));
    ADC_StopConv();
    DMA_ClearITPendingBit(DMA_CH1);  
    DMA_ClearITPendingBit(DMA_CH2);
    max1 = min1 =max2=min2=0;
    for(int i = 0;i<100;i++)
    {
      if(ad1[i]>ad1[max1])
      {
        max1 = i;
      }
      else if(ad1[i]<ad1[min1])
      {
        min1 = i;
      }
      if(ad3[i]>ad3[max2])
      {
        max2 = i;
      }
      else if(ad3[i]<ad3[min2])
      {
        min2 = i;
      }
    }
    //OLED_PrintfAt(FONT_ASCII_6X8,0,0,"AD phase: %f\n ",(ad1[max1]+ad1[min1])/2);         //在指定位置打印 
    //OLED_PrintfAt(FONT_ASCII_6X8,10,0,"AD range: %f\n ",(ad3[max2]+ad3[min2])/2);         //在指定位置打印 
    //for(int i=0;i<1023;i++)
    //printf("%d %f %f\n",i,ad1[i]*3.3/1024,ad3[i]*3.3/1024);
    OLED_Clear();//清屏
    V_Phase = (ad1[max1]+ad1[min1])/2;
    Vr=(ad3[max2]+ad3[min2])/2;
    /*
    Phase=(-0.2981*V_Phase + 175.54)/180*PI;
    VrdVx=(9.2528*Vx - 6.6667)/(9.2528*Vr - 6.6667);
    R_Value=VrdVx*Rr*cos(Phase)-Rr;
    X_Value=VrdVx*Rr*sin(Phase);
    OLED_DispFolatAt(FONT_ASCII_6X8,0,0,(ad1[max1]+ad1[min1])/2,6);
    //OLED_DispFolatAt(FONT_ASCII_6X8,2,0,(ad3[max2]+ad3[min2])/2,6);
    
    if(flag_LC)
    {
      OLED_DispStringAt(FONT_ASCII_6X8,1,0,"capacitor measure");
      OLED_DispStringAt(FONT_ASCII_6X8,2,0,"C (nF) :");
      C_Value=1000000000/2/PI/Fre1/X_Value;
      OLED_DispFolatAt(FONT_ASCII_6X8,3,0,C_Value,6);
      OLED_DispStringAt(FONT_ASCII_6X8,4,0,"D  :");
      D_Value=R_Value/X_Value;
      OLED_DispFolatAt(FONT_ASCII_6X8,5,0,D_Value,6);
      LED_SetStatus(LED1, TRUE);                                        //设置LED状态
      LED_SetStatus(LED2, TRUE);                                        //设置LED状态
 
      
      
    }
    else
    {
      OLED_DispStringAt(FONT_ASCII_6X8,1,0,"inductance measure");
      OLED_DispStringAt(FONT_ASCII_6X8,2,0,"L (uH) :");
      L_Value=1000000*X_Value/2/PI/Fre2;
      OLED_DispFolatAt(FONT_ASCII_6X8,3,0,L_Value,6);
      OLED_DispStringAt(FONT_ASCII_6X8,4,0,"Q  :");
      Q_Value=X_Value/R_Value;
      OLED_DispFolatAt(FONT_ASCII_6X8,5,0,Q_Value,6);
      LED_SetStatus(LED1, FALSE);                                        //设置LED状态
      LED_SetStatus(LED2, FALSE);                                        //设置LED状态 
    }
    */
    OLED_DispStringAt(FONT_ASCII_6X8,3,58,"texting...");
    //暂停程序
    
    if(count<30)
    {
      count++;//可以在这里对测量的数据进行处理
      //相位电压取最大值
      buffer_P=fmax(buffer_P,V_Phase);
      //幅度电压取最大值
      buffer_V=fmin(buffer_V,Vr);
    }
    else
    {
      OLED_Clear();
      buffer_P+=6;
      buffer_V+=6;
      Phase=(-0.2981*buffer_P + 175.54)/180*PI;
      Vr=buffer_V;
      Vx_Value=9.2528*Vx - 6.6667;
      Vr_Value=9.2528*buffer_V - 6.6667;
      
    //VrdVx=(9.2528*Vx - 6.6667)/(9.2528*buffer_V - 6.6667);
    /*R_Value=VrdVx*Rr*cos(Phase)-Rr;
    X_Value=VrdVx*Rr*sin(Phase);*/
      Vz=sqrt(Vx_Value*Vx_Value+Vr_Value*Vr_Value-2*Vr_Value*Vx_Value*cos(Phase));
      Z_Value=Rr*Vz/Vr_Value;
      Sin_Z=sin(Phase)*Vx_Value/Vz;
      X_Value=Z_Value*Sin_Z;
      R_Value=Z_Value*cos(asin(Sin_Z));
      
    //OLED_DispFolatAt(FONT_ASCII_6X8,0,0,(ad1[max1]+ad1[min1])/2,6);
    //OLED_DispFolatAt(FONT_ASCII_6X8,6,0,(ad3[max2]+ad3[min2])/2,6);
    
    if(flag_LC)
    {
      OLED_DispStringAt(FONT_ASCII_6X8,1,0,"capacitor measure");
      OLED_DispStringAt(FONT_ASCII_6X8,2,0,"C (nF) :");
      C_Value=1000000000/2/PI/Fre1/X_Value*0.89;
      if(C_Value<7.5&&C_Value>4)
      {
       C_Value = 1.1158*C_Value - 1.3112;
      }
      if(C_Value<3.5)
      {
        C_Value=0.8591*C_Value - 0.6144;
      }
      OLED_DispFolatAt(FONT_ASCII_6X8,3,0,C_Value,6);
      //OLED_DispFolatAt(FONT_ASCII_6X8,4,0,Vz,6);
      OLED_DispStringAt(FONT_ASCII_6X8,4,0,"D  :");
      D_Value=R_Value/X_Value;
      OLED_DispFolatAt(FONT_ASCII_6X8,5,0,D_Value,6);
      LED_SetStatus(LED1, TRUE);                                        //设置LED状态
      LED_SetStatus(LED2, TRUE);                                        //设置LED状态
 
      
      
    }
    else
    {
      OLED_DispStringAt(FONT_ASCII_6X8,1,0,"inductance measure");
      OLED_DispStringAt(FONT_ASCII_6X8,2,0,"L (uH) :");
      L_Value=1000000*X_Value/2/PI/Fre2;
      if(L_Value<20)
      {
        L_Value=L_Value*1.17;
      }
      else if(L_Value<20)
      {
        L_Value=L_Value*1.13;
        
      }
      else if(L_Value<60)
      {
        L_Value=L_Value*1.12;
      }
      else
      {
        L_Value=L_Value*1.05;
      }
      OLED_DispFolatAt(FONT_ASCII_6X8,3,0,L_Value,6);
      OLED_DispStringAt(FONT_ASCII_6X8,4,0,"Q  :");
      Q_Value=X_Value/R_Value;
      OLED_DispFolatAt(FONT_ASCII_6X8,5,0,Q_Value,6);
      LED_SetStatus(LED1, FALSE);                                        //设置LED状态
      LED_SetStatus(LED2, FALSE);                                        //设置LED状态 
    }
      
      OLED_DispStringAt(FONT_ASCII_6X8,7,0,"COMPLETE");
      while(flag)
      {;}
      flag=1;
      count=0;
      buffer_P=0;
      buffer_V=1000;
    }
    
    //delay_us(100000);
    //for(int i=0;i<100;i++)
    //printf("%d %f %f\n",0,ad1[50]*3.3/1023,ad3[50]*3.3/1023);
  }
}







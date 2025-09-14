  /**
  ******************************************************************************
  * @file    GUI_App.c
  * @author  MCD Application Team
  * @brief   Simple demo drawing "Hello world"  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2018 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#include "GUI_App.h"
#include "GUI.h"

#include "DIALOG.h"
extern  WM_HWIN CreateWindow(void);  
  

void GRAPHICS_MainTask(void) {

  /* 2- Create a Window using GUIBuilder */
  CreateWindow();
 
/* USER CODE BEGIN GRAPHICS_MainTask */
 /* User can implement his graphic application here */
  /* Hello Word example */
//    GUI_Clear();
//    GUI_SetColor(GUI_WHITE);
//    GUI_SetFont(&GUI_Font32_1);
//    GUI_DispStringAt("Hello world!", (LCD_GetXSize()-150)/2, (LCD_GetYSize()-20)/2);
  
    extern void checkTouch(void);
    extern struct status state;
    extern void send(int data[17]);
    extern int testdatabase[17];
    extern bool send_flag;//,music_flag,play_music_flag;
    extern int after_diff[17];
    void diff_code(int data[17]);
    //void change_send_data(void);
    void stop_send(void);
    
//    uint8_t BSP_AUDIO_OUT_Resume(void);
//    uint8_t BSP_AUDIO_OUT_Pause(void);
//    void play_music(void);
    while(1)
    {
        checkTouch();
        if(send_flag)
        {
//            send_flag=0;
            testdatabase[0]=state.LED;
            testdatabase[1]=state.num>>3;
            testdatabase[2]=(state.num>>2)&0x0001;
            testdatabase[3]=(state.num>>1)&0x0001;
            testdatabase[4]=state.num&0x01;
            testdatabase[5]=state.RGB[0];
            testdatabase[6]=state.RGB[1];
            testdatabase[7]=state.RGB[2];
            testdatabase[8]=state.color[0]>>2;
            testdatabase[9]=(state.color[0]>>1)&0x0001;
            testdatabase[10]=state.color[0]&0x0001;
            testdatabase[11]=state.color[1]>>2;
            testdatabase[12]=(state.color[1]>>1)&0x0001;
            testdatabase[13]=state.color[1]&0x0001;
            testdatabase[14]=state.color[2]>>2;
            testdatabase[15]=(state.color[2]>>1)&0x0001;
            testdatabase[16]=state.color[2]&0x0001;
//            for(int i=0;i<17;i++)
//            {
//                testdatabase[i]=0;
//            }
//            for(int i=5;i<13;i++)
//            {
//                testdatabase[i]=1;
//            }
            diff_code(testdatabase);
//            send(after_diff); 
            send_flag=0;
//            change_send_data();
        }
        send(after_diff); 
        GUI_Delay(29);
    }
/* USER CODE END GRAPHICS_MainTask */
  while(1)
{
      GUI_Delay(100);
}
}

/*************************** End of file ****************************/

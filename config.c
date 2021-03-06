#include "Config.h"
/***********************************************************************
                   系统时钟初始化
***********************************************************************/
void Clock_Init()
{
  unsigned char i;
  BCSCTL1&=~XT2OFF;                 //打开XT2振荡器
  BCSCTL2|=SELM_2+SELS+DIVS_3;              //MCLK为8MHZ，SMCLK为1MHZ
  do{
    IFG1&=~OFIFG;                   //清楚振荡器错误标志
    for(i=0;i<100;i++)
       _NOP();
  }
  while((IFG1&OFIFG)!=0);           //如果标志位1，则继续循环等待
  IFG1&=~OFIFG;
}
/***********************************************************************
               MSP430内部看门狗初始化
***********************************************************************/
void WDT_Init()
{
   WDTCTL = WDTPW + WDTHOLD;       //关闭看门狗
}
/***********************************************************************
               端口初始化
***********************************************************************/
void Port_Init()
{
    P1DIR=0X00;P1OUT=0Xff;
    P2SEL = 0x00;                        //键盘的IO口设置为普通I/O模式，此句可省      
    P2DIR = 0x0F;                        //高四位输入模式，低四位输出模式，外部上拉电阻
    P2OUT = 0xF0;                        //初始值0xF0，为什么要这样？准备可中断的条件
    P2IES =0xf0;                         //P1.4~P1.7下降沿触发中断
    P2IE  =0xf0;                         //P1.4~P1.7允许中断
    P2IFG=0x00;                          //中断标志清0
    P3DIR=0XFF;P3OUT=0XFF;
    P4DIR=0XFF;P4OUT=0XFF;
    P5DIR=0XFF;P5OUT=0XFF;
    P6DIR=0X00;P6OUT=0XFF;
}
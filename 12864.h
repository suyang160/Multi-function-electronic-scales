/********************************************
文件名：12864.c
编写者：xdq
描述：12864模块程序。用于14xx系列。
MCLK：12m   SCLK：4kHz   ACLK: 
版本：2012-6
********************************************/
#ifndef  _12864_H
#define  _12864_H
#include <msp430x14x.h>
#include <math.h>
#include  "delay.h"

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

//12864端口设置
#define LCD_DataIn    P2DIR=0x00    //数据口方向设置为输入
#define LCD_DataOut   P2DIR=0xff    //数据口方向设置为输出
#define LCD2MCU_Data  P2IN         // 数据端口
#define MCU2LCD_Data  P2OUT
#define LCD_CMDOut    P3DIR|= BIT1 + BIT2 + BIT3;     //P3口的控制口设为输出

#define LCD_RS_H      P3OUT|=BIT1      //P3.1
#define LCD_RS_L      P3OUT&=~BIT1     //P3.1
#define LCD_RW_H      P3OUT|=BIT2      //P3.2
#define LCD_RW_L      P3OUT&=~BIT2     //P3.2
#define LCD_EN_H      P3OUT|=BIT3      //P3.3
#define LCD_EN_L      P3OUT&=~BIT3     //P3.3

void  Init_Lcd(void);
void  Write_Cmd(uchar cmd);
void  Write_Data(uchar dat);

#endif
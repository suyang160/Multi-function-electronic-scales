/********************************************
�ļ�����12864.c
��д�ߣ�xdq
������12864ģ���������14xxϵ�С�
MCLK��12m   SCLK��4kHz   ACLK: 
�汾��2012-6
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

//12864�˿�����
#define LCD_DataIn    P2DIR=0x00    //���ݿڷ�������Ϊ����
#define LCD_DataOut   P2DIR=0xff    //���ݿڷ�������Ϊ���
#define LCD2MCU_Data  P2IN         // ���ݶ˿�
#define MCU2LCD_Data  P2OUT
#define LCD_CMDOut    P3DIR|= BIT1 + BIT2 + BIT3;     //P3�ڵĿ��ƿ���Ϊ���

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
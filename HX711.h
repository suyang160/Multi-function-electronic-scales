#ifndef __HX711_H__
#define __HX711_H__


#include  <msp430x14x.h>

//IO设置     DOUT(P1.0)   SCK(P1.1)
//DOUT   输入    SCK    输出  

//#define  HX711_CMDOut_1      P5DIR=0x03           // SCK(输出）DOUT(输出） 
#define  HX711_CMDOut_2      P5DIR=0x02           // SCK(输出）DOUT(输入）
#define  HX711_DOUT_H        P5OUT|=BIT0
#define  HX711_DOUT_L        P5OUT&=~BIT0
#define  HX711_SCK_H         P5OUT|=BIT1
#define  HX711_SCK_L         P5OUT&=~BIT1



//函数或者变量声明
extern void Delay__hx711_us(void);
extern unsigned long HX711_Read(void);

#endif
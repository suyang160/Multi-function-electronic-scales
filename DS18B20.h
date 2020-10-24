/*DS18B20管脚定义*/
#ifndef  DS18B20_H
#define  DS18B20_H

#include <msp430x14x.h>
#include  "delay.h"
#define DQ_IN	        P5DIR &= ~BIT3		  //设置输入，DS18B20接单片机P53口
#define DQ_OUT	        P5DIR |= BIT3		  //设置输出
#define DQ_CLR	        P5OUT &= ~BIT3	          //置低电平
#define DQ_SET	        P5OUT |= BIT3	          //置高电平
#define DQ_R	        P5IN & BIT3		  //读电平
#ifndef uint
#define uint unsigned int
#endif
extern unsigned int  A1;
extern unsigned int  A2;                          //<---DS18B20的三个显示位
extern unsigned int  A3;
extern unsigned int    temp_value;
extern void ds1820_start(void);
extern unsigned int ds1820_read_temp(void) ;
extern void data_do(uint temp_d);


#endif

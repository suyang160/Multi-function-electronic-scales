/*DS18B20�ܽŶ���*/
#ifndef  DS18B20_H
#define  DS18B20_H

#include <msp430x14x.h>
#include  "delay.h"
#define DQ_IN	        P5DIR &= ~BIT3		  //�������룬DS18B20�ӵ�Ƭ��P53��
#define DQ_OUT	        P5DIR |= BIT3		  //�������
#define DQ_CLR	        P5OUT &= ~BIT3	          //�õ͵�ƽ
#define DQ_SET	        P5OUT |= BIT3	          //�øߵ�ƽ
#define DQ_R	        P5IN & BIT3		  //����ƽ
#ifndef uint
#define uint unsigned int
#endif
extern unsigned int  A1;
extern unsigned int  A2;                          //<---DS18B20��������ʾλ
extern unsigned int  A3;
extern unsigned int    temp_value;
extern void ds1820_start(void);
extern unsigned int ds1820_read_temp(void) ;
extern void data_do(uint temp_d);


#endif

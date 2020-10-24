#include "HX711.h"
#include <msp430x14x.h>
//****************************************************
//延时函数
//****************************************************
void Delay__hx711_us(void)
{
	_NOP();
	_NOP();
}

//****************************************************
//读取HX711
//****************************************************
unsigned long HX711_Read(void)	//增益128
{
	unsigned long count; 
	unsigned char i;
  	HX711_DOUT_H; 
	Delay__hx711_us();
  	HX711_SCK_L; 
  	count=0;
        HX711_CMDOut_2;
  	while(P5IN&0x01); 
  	for(i=0;i<24;i++)
	{ 
	  	HX711_SCK_H; 
	  	count=count<<1; 
		HX711_SCK_L; 
	  	if(P5IN&0x01)
		count++; 
	} 
 	HX711_SCK_H; 
	count=count^0x800000;
	HX711_SCK_L;  
	return(count);
}







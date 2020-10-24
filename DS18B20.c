#include  <msp430x14x.h>
#include  "DS18B20.h"

//************************************************************************* 
//			DS18B20初始化
//*************************************************************************
unsigned int    temp_value;
unsigned int    temp,A1,A2,A3;             		  //定义的变量,显示数据处理


unsigned char DS18B20_Reset(void)                //初始化和复位
{
  unsigned char i;
  DQ_OUT;
  DQ_CLR;
  delay_us(500);				//延时500uS(480-960)
  DQ_SET;
  DQ_IN;
  delay_us(80);				        //延时80uS
  i = DQ_R;
  delay_us(500);				//延时500uS(保持>480uS)
	
  if (i) 
  {
    return 0x00;
  }
  else 
  {
    return 0x01;
  }
}

//************************************************************************* 
//			DS18B20读一个字节函数
//************************************************************************* 
   
unsigned char ds1820_read_byte(void) 
{
  unsigned char i;
  unsigned char value = 0;
  for (i = 8; i != 0; i--) 
  {
    value >>= 1;
    DQ_OUT;
    DQ_CLR;
    delay_us(4);			        //*延时4uS	
    DQ_SET;
    DQ_IN;
    delay_us(10);			        //*延时10uS	
    if (DQ_R) 
    {
      value|=0x80;		
    }
    delay_us(60);			        //*延时60uS	
  }
  return(value);
}

//************************************************************************* 
//			向18B20写一个字节函数
//*************************************************************************  

/*DS18B20字节写入函数*/
void ds1820_write_byte(unsigned char value) 
{
  unsigned char i;
  for (i = 8; i != 0; i--) 
  {
    DQ_OUT;
    DQ_CLR;
    delay_us(4);			  //延时4uS
    if (value & 0x01) 
    {
      DQ_SET;		
    }
    delay_us(80);			  //延时80uS
    DQ_SET;			          //位结束
    value >>= 1;	
  }
}
//*************************************************************************
//				发送温度转换命令
//************************************************************************* 

/*启动ds1820转换*/
void ds1820_start(void) 
{
  DS18B20_Reset();
  ds1820_write_byte(0xCC);	          //勿略地址
  ds1820_write_byte(0x44);	          //启动转换
}

//*************************************************************************
//				DS8B20读取温度信息
//************************************************************************* 

unsigned int ds1820_read_temp(void) 
{
  unsigned int i;
  unsigned char buf[9];

  DS18B20_Reset();
  ds1820_write_byte(0xCC);	          //勿略地址
  ds1820_write_byte(0xBE);	          //读取温度
  for (i = 0; i < 9; i++) 
  {
    buf[i] = ds1820_read_byte();	
  }
  i = buf[1];
  i <<= 8;
  i |= buf[0];
  temp_value=i;
  temp_value=(uint)(temp_value*0.625);    //不是乘以0.0625的原因是为了把小数点后一位数据也转化为可以显示的数据
		                          //比如温度本身为27.5度，为了在后续的数据处理程序中得到BCD码，我们先放大到275
                                          //然后在显示的时候确定小数点的位置即可，就能显示出27.5度了
  return temp_value;
}
 void data_do(uint temp_d)
 {
   uint A2t;
   A1=temp_d/100;                         //分出百，十，和个位
   A2t=temp_d%100;
   A2=A2t/10;
   A3=A2t%10;
}

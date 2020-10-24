#include  <msp430x14x.h>
#include  "DS18B20.h"

//************************************************************************* 
//			DS18B20��ʼ��
//*************************************************************************
unsigned int    temp_value;
unsigned int    temp,A1,A2,A3;             		  //����ı���,��ʾ���ݴ���


unsigned char DS18B20_Reset(void)                //��ʼ���͸�λ
{
  unsigned char i;
  DQ_OUT;
  DQ_CLR;
  delay_us(500);				//��ʱ500uS(480-960)
  DQ_SET;
  DQ_IN;
  delay_us(80);				        //��ʱ80uS
  i = DQ_R;
  delay_us(500);				//��ʱ500uS(����>480uS)
	
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
//			DS18B20��һ���ֽں���
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
    delay_us(4);			        //*��ʱ4uS	
    DQ_SET;
    DQ_IN;
    delay_us(10);			        //*��ʱ10uS	
    if (DQ_R) 
    {
      value|=0x80;		
    }
    delay_us(60);			        //*��ʱ60uS	
  }
  return(value);
}

//************************************************************************* 
//			��18B20дһ���ֽں���
//*************************************************************************  

/*DS18B20�ֽ�д�뺯��*/
void ds1820_write_byte(unsigned char value) 
{
  unsigned char i;
  for (i = 8; i != 0; i--) 
  {
    DQ_OUT;
    DQ_CLR;
    delay_us(4);			  //��ʱ4uS
    if (value & 0x01) 
    {
      DQ_SET;		
    }
    delay_us(80);			  //��ʱ80uS
    DQ_SET;			          //λ����
    value >>= 1;	
  }
}
//*************************************************************************
//				�����¶�ת������
//************************************************************************* 

/*����ds1820ת��*/
void ds1820_start(void) 
{
  DS18B20_Reset();
  ds1820_write_byte(0xCC);	          //���Ե�ַ
  ds1820_write_byte(0x44);	          //����ת��
}

//*************************************************************************
//				DS8B20��ȡ�¶���Ϣ
//************************************************************************* 

unsigned int ds1820_read_temp(void) 
{
  unsigned int i;
  unsigned char buf[9];

  DS18B20_Reset();
  ds1820_write_byte(0xCC);	          //���Ե�ַ
  ds1820_write_byte(0xBE);	          //��ȡ�¶�
  for (i = 0; i < 9; i++) 
  {
    buf[i] = ds1820_read_byte();	
  }
  i = buf[1];
  i <<= 8;
  i |= buf[0];
  temp_value=i;
  temp_value=(uint)(temp_value*0.625);    //���ǳ���0.0625��ԭ����Ϊ�˰�С�����һλ����Ҳת��Ϊ������ʾ������
		                          //�����¶ȱ���Ϊ27.5�ȣ�Ϊ���ں��������ݴ�������еõ�BCD�룬�����ȷŴ�275
                                          //Ȼ������ʾ��ʱ��ȷ��С�����λ�ü��ɣ�������ʾ��27.5����
  return temp_value;
}
 void data_do(uint temp_d)
 {
   uint A2t;
   A1=temp_d/100;                         //�ֳ��٣�ʮ���͸�λ
   A2t=temp_d%100;
   A2=A2t/10;
   A3=A2t%10;
}

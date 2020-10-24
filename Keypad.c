#include  <msp430x14x.h>
#include  "Keypad.h"
#include  "delay.h"
typedef unsigned char uchar;
typedef unsigned int  uint;

/***************ȫ�ֱ���***************/
uchar key_Pressed;      //�����Ƿ񱻰���:1--�ǣ�0--��
uchar key_val;          //��ż�ֵ//
uchar key_Flag;         //�����Ƿ��ѷſ���1--�ǣ�0--��
//���ü����߼���ֵ���������ֵ��ӳ��
uchar key_Map[30] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};

/*******************************************
�������ƣ�Init_Keypad
��    �ܣ���ʼ��ɨ����̵�IO�˿�
��    ������
����ֵ  ����
********************************************/
void Init_Keypad(void)
{    
    P1SEL = 0x00; 
    P1DIR = 0xff;       //P1����Ϊ���
    P1SEL = 0x00; 
    P4DIR = 0x00;       //P4.0~4����Ϊ����
    P4OUT = 0xff;
    P1OUT |= 0xff;      // P1.0~P1.5����ߵ�ƽ
    key_Flag = 0;       
    key_Pressed = 0;   
    key_val = 0;
}
/*******************************************
�������ƣ�Check_Key
��    �ܣ�ɨ����̵�IO�˿ڣ���ü�ֵ
��    ������
����ֵ  ����
********************************************/

void Check_Key(void)
{
    uchar row ,col,tmp1,tmp2;
    
    tmp1 = 0x01;
    for(col = 0;col < 6;col++)              //��ɨ��
    {
        P1OUT = 0x3f;                      //P1.0~P1.5���ȫ1
        P1OUT -= tmp1;                      //P1.0~P1.5�����λ����һ��Ϊ0
        tmp1 <<=1;                              
        if ((P4IN & 0x1f) < 0x1f)           //�Ƿ�P4IN��P4.0~P4.4����һλΪ0
        {                       
            tmp2 = 0x01;                         // tmp2���ڼ�����һλΪ0  
            for(row = 0;row < 5;row ++)              // �м�� 
            {          
                if((P4IN & tmp2) == 0x00)           // �Ƿ��Ǹ���,����0Ϊ��
                {          
                    key_val = key_Map[row * 6 + col];  // ��ȡ��ֵ
                    while((P4IN& 0x1f)<0x1f)
                    {
                      while((P4IN& 0x1f) ==0x1f)
                      {
                        return;                         // �˳�ѭ��
                      }
                    }
                }
                tmp2 <<= 1;                        // tmp2����1λ 
            }
        }
     }
}
/*******************************************
�������ƣ�delay
��    �ܣ���ʱԼ15ms�������������
��    ������
����ֵ  ����
********************************************/
void delay()
{
    uint tmp;
     
    for(tmp = 12000;tmp > 0;tmp--);
}
/*******************************************
�������ƣ�Key_Event
��    �ܣ���ⰴ��������ȡ��ֵ
��    ������
����ֵ  ����
********************************************/
void Key_Event()
{     
    uchar tmp;
     
    P1OUT &= 0x00;              // ����P1OUTȫΪ0���ȴ���������
    tmp = P4IN;                 // ��ȡP4IN
    if ((key_Pressed == 0)&&((tmp & 0x1f) < 0x1f)) //����м�����
    {                     
        key_Pressed = 1;        // ����а������£�����key_Pressed��ʶ
        delay();                //��������
        Check_Key();            // ����check_Key(),��ȡ��ֵ
     }     
    else if ((key_Pressed == 1)&&((tmp & 0x1f) == 0x1f)) //��������Ѿ��ͷ�
    {                     
        key_Pressed = 0;        // ���key_Pressed��ʶ
        key_Flag    = 1;        // ����key_Flag��ʶ
     }
    else 
    {
        _NOP();
    }     
}

unsigned char key_falg()         //�ú����ڼ��̳�ʼ��������а������£��򷵻�1������Ϊ0
{
    P1OUT &= 0x00;              // ����P1OUTȫΪ0���ȴ���������
    if((P4IN & 0x1f) <0x1f)
    {
      delay_ms(20);             //��ֹ����
      if((P4IN & 0x1f) <0x1f)
      {
       return 1;
      }
      else
      {
        return 0;
      }
    }
    else
    {
      return 0;
    }
  
}


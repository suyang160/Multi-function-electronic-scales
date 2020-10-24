#include "12864.h"
/*******************************************
�������ƣ�Write_Cmd
��    �ܣ���Һ����д��������
��    ����cmd--��������
����ֵ  ����
********************************************/
void Write_Cmd(uchar cmd)
{
    uchar lcdtemp = 0;
			
    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;  
    do                       //��æ
    {    
        LCD_EN_H;
       	_NOP();					
       	lcdtemp = LCD2MCU_Data; 
       	LCD_EN_L;
        
    }
    while(lcdtemp & 0x80); 
    
    
    LCD_DataOut;    
    LCD_RW_L;  		
    MCU2LCD_Data = cmd; 
    LCD_EN_H;
    _NOP();						  			
    LCD_EN_L;
}
/*******************************************
�������ƣ�Write_Data
��    �ܣ���Һ����д��ʾ����
��    ����dat--��ʾ����
����ֵ  ����
********************************************/
void  Write_Data(uchar dat)
{
    uchar lcdtemp = 0;   
        
    LCD_RS_L;
    LCD_RW_H;  
    LCD_DataIn;   
    do                       //��æ
    {    
        LCD_EN_H;
        _NOP();						
        lcdtemp = LCD2MCU_Data; 
        LCD_EN_L;      
    }
    while(lcdtemp & 0x80);  
    
    LCD_DataOut; 
    LCD_RS_H;
    LCD_RW_L;  
        
    MCU2LCD_Data = dat;
    LCD_EN_H;
    _NOP();
    LCD_EN_L;
}  
/*******************************************
�������ƣ�Ini_Lcd
��    �ܣ���ʼ��Һ��ģ��
��    ������
����ֵ  ����
********************************************/
void Init_Lcd(void)
{                  
    LCD_CMDOut;    //Һ�����ƶ˿�����Ϊ���

    //Delay_Nms(100);
    Write_Cmd(0x30);   //����ָ�
    delay_ms(1);
    Write_Cmd(0x02);   // ��ַ��λ
    delay_ms(1);
	Write_Cmd(0x0c);   //������ʾ��,�α�ر�
    delay_ms(1);
	Write_Cmd(0x01);   //�����ʾ
    delay_ms(1);
	Write_Cmd(0x06);   //�α�����
    delay_ms(1);
	Write_Cmd(0x80);   //�趨��ʾ����ʼ��ַ
}

/*******************************************
�������ƣ�Disp_HZ
��    �ܣ�����Һ����ʾ����
��    ����addr--��ʾλ�õ��׵�ַ
          pt--ָ����ʾ���ݵ�ָ��
          num--��ʾ�ַ�����
����ֵ  ����
********************************************/
void Disp_HZ(uchar addr,const char * pt,uchar num)
{
    uchar i;
		
    Write_Cmd(addr); 
   	for(i = 0;i < (num);i++) 
        Write_Data(*(pt++)); 
} 
/*******************************************
�������ƣ�Disp_3num
��    �ܣ���LCDָ��λ��д��������
��    ����addr--��ʾλ�õ��׵�ַ
          num--ָ����ʾ����λ��
����ֵ  ����
********************************************/
void Disp_4num(uchar addr,uint num)
{
    Write_Cmd(addr);
    Write_Data(num%100000/10000+0x30);     //��λ    
    Write_Data(num%10000/1000+0x30);       //ǧλ    
    Write_Data(num%1000/100+0x30);         //ǧλ
    Write_Data(num%100/10+0x30);           //ʮλ
    Write_Data(num%10+0x30);               //��λ
}

void Disp_3num(uchar addr,int num)
{
  if(num>=0)
  {    
    Write_Cmd(addr-1);
    Write_Data(0x2b);
    Write_Data(num%1000/100+0x30);         //ǧλ
    Write_Data(num%100/10+0x30);           //ʮλ
    Write_Data(num%10+0x30);               //��λ
  }
  if(num<0)
  {
    Write_Cmd(addr-1);
    Write_Data(0x2d);
    Write_Data((-num)%1000/100+0x30);         //ǧλ
    Write_Data((-num)%100/10+0x30);           //ʮλ
    Write_Data((-num)%10+0x30);               //��λ
  }  
  
}

void Disp_2num(uchar addr,uint num)
{
    Write_Cmd(addr);   
    Write_Data(num%1000/100+0x30);         //ǧλ
    Write_Data(num%100/10+0x30);           //ʮλ
    Write_Data(num%10+0x30);               //��λ
}

/*******************************************
�������ƣ�lcd12864_image
��    �ܣ���LCD����ʾ����ͼ��
��    ����*p--��ʾ���� 64*16�ֽ� 
��д�ߣ�lh  2013-5-16      
����ֵ  ����
˵���� ��������� 8*8�ֽ�ͼ���ע����Ĳ���
********************************************/
void lcd12864_image(const unsigned char *p)
{
   int x,y;
   Write_Cmd(0x34); //��չָ�����,��ͼ��ʾOFF
  for (y=0;y<32;y++) //���ϰ�����ʾ����
     {
      Write_Cmd(0x80|y); //����ʾY����0x80-0x9f
      Write_Cmd(0x80); //����ʾX����0x80
   for (x=0;x<16;x++)
        {
          Write_Data(*p);
             p++;
        }
     }
  
for (y=0;y<32;y++) //���°�����ʾ����
   {
    Write_Cmd(0x80|y); //����ʾY����0x80-0x9f
    Write_Cmd(0x88); //����ʾX����0x88
   for (x=0;x<16;x++)
     {
      Write_Data(*p);
       p++;
     }
   }
   Write_Cmd(0x36); //��չָ�����,��ͼ��ʾON ע�������費��Ҫ�ص�����ָ��
    Delay_Nms(50);
   Write_Cmd(0x30); //�ص�����ָ�� 
}

/*******************************************
�������ƣ�gui_clear
��    �ܣ���ͼģʽ�µ�����
��    ������
��д�ߣ�lh  2013-5-16      
����ֵ  ����
˵���� ��ͼģʽ�޷�ʹ��Write_Cmd(0x01)����
********************************************/
void gui_clear(void)
{
   int x,y;
   Write_Cmd(0x34); //��չָ�����,��ͼ��ʾOFF
  for (y=0;y<32;y++) //���ϰ�����ʾ����
     {
      Write_Cmd(0x80|y); //����ʾY����0x80-0x9f
      Write_Cmd(0x80); //����ʾX����0x80
   for (x=0;x<16;x++)
        {
          Write_Data(0x00);
        }
     }
  
for (y=0;y<32;y++) //���°�����ʾ����
   {
    Write_Cmd(0x80|y); //����ʾY����0x80-0x9f
    Write_Cmd(0x88); //����ʾX����0x88
   for (x=0;x<16;x++)
     {
      Write_Data(0x00);
     }
   }
   Write_Cmd(0x36); //��չָ�����,��ͼ��ʾON ע�������費��Ҫ�ص�����ָ��
    delay_ms(50);
   Write_Cmd(0x30); //�ص�����ָ�� 
}
/*******************************************
�������ƣ�ReadByte_f
��    �ܣ���LCD�϶�һ���ֽ�������״̬
��    ������ 
��д�ߣ�lh  2013-5-16      
����ֵ  ��һ���ֽڵ�״̬����DDRAMֵ��8λ
˵���� ��  0000 0000 ����ϵ�ַʹ��)��ȷ��ĳһ���ֽڵ���ֵ
********************************************/
unsigned char ReadByte_f( void )
{
    unsigned char byReturnValue ;     // ����һ���޷��ŵ��ַ��ͱ���Ϊ���״̬
    uchar lcdtemp = 0;		
    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;  
    do                       //��æ
    {    
        LCD_EN_H;
       	_NOP();					
       	lcdtemp = LCD2MCU_Data; 
       	LCD_EN_L;   
    }
    while(lcdtemp & 0x80); 
    
    LCD_RS_H;                // RS=1������
    LCD_RW_H;                // R/W=1��������
    _NOP();		              
    LCD_EN_L;                  //E�ȵ͵�ƽ���ٸߵ�ƽ��������
    LCD_EN_H;
    byReturnValue = LCD2MCU_Data ;   //������
    LCD_EN_L;             // �ر�ʹ��
    LCD_CMDOut;      //Һ�����ƶ˿�����Ϊ��� 
      
    return byReturnValue ;    // ���ض������ݸ������ӿ���
}
/*****************************************0,0        127,0
�������ƣ�DrawPoint_f дDDRAMֵ            |           |
��    �ܣ���LCD�Ͼ�������λ�û�һ����      |           |
��    ����X,Y,Color                        |           |
           X---�кţ�Y--�к�              0,63        127,63
     Color--0 �����㣻1--���㣻2--��򣬵���ǰ�������ڲ�������֮�෴��
��д�ߣ�lh  2013-5-16      
����ֵ����
˵���� TierΪ�ж�Ӧ���ֽڵ�ַ��0-16��Tier_bitΪ���ֽ����λ����0-7��8λ����
********************************************/
void DrawPoint_f( unsigned char X, unsigned char Y, unsigned char Color )
{
    unsigned char Row , Tier , Tier_bit    ;  // ���廭������꣬XΪ��0-127��YΪ��0-63
    unsigned char ReadOldH, ReadOldL ;      // �����������״̬
    Write_Cmd(0x34) ;         // ����ָ���չָ����л�
    	_NOP();
    Write_Cmd(0x36);         // ��ͼ��ʾ��
    Tier = X >> 4 ;           //�ұ��ƶ�4λ��ȡ��λ�����Tier-�ж�Ӧ���ֽڵ�ַ
    Tier_bit = X & 0x0f ;     //Tier_bitΪ���ֽ����λ����0-7��8λ����
    if( Y < 32 )
    {
        Row = Y ;        // �ϰ��� 80H���е�ַ
    }
    else
    {
        Row = Y - 32 ;   // �°������е�ַ
        Tier += 8 ;      //  88H  �ֵ�ַ
    }
    Write_Cmd(Row+0x80) ;   // �е�ַ
    Write_Cmd(Tier+0x80) ; //  �е�ַ
      ReadByte_f() ;       //����λ�õ�״̬
    ReadOldH = ReadByte_f() ; //����λ�õ�״̬
    ReadOldL = ReadByte_f() ; //����λ�õ�״̬
    Write_Cmd(Row+0x80)    ; //
    Write_Cmd(Tier+0x80) ;  //
    if( Tier_bit < 8 )
    {
        switch( Color)   //1 ��λ��0 Ϊ��0�� 2 ���
        {
            case 0 : ReadOldH &=( ~( 0x01 << ( 7 - Tier_bit ))) ; break ;
            case 1 : ReadOldH |= ( 0x01 << ( 7 - Tier_bit )) ; break ;
            case 2 : ReadOldH ^= ( 0x01 << ( 7 - Tier_bit ))    ; break ;
            default : break ;    
        }
        Write_Data( ReadOldH ) ;
        Write_Data( ReadOldL ) ;
    }
    else
    {
        switch(Color)
        {
            case 0 : ReadOldL &= (~( 0x01 << ( 15 - Tier_bit ))) ; break ;
            case 1 : ReadOldL |= ( 0x01 << ( 15 - Tier_bit ))    ; break ;
            case 2 : ReadOldL ^= ( 0x01 << ( 15 - Tier_bit )) ; break ;
            default : break ;
        }
        Write_Data( ReadOldH ) ;
        Write_Data( ReadOldL ) ;
    }
    Write_Cmd(0x30); //�ص�����ָ��   ;
}
/*******************************************
�������ƣ�DrawLineX_f
��    �ܣ���LCDˮƽ�����ϻ�һ��ֱ��
��    ����X0-ˮƽ������ʼ�кţ�X1-ˮƽ������ֹ�кţ�
         Y-�кţ� Color--����
��д�ߣ�lh  2013-5-16      
����ֵ����
˵���� ��
********************************************/
void DrawLineX_f( unsigned char X0, unsigned char X1, unsigned char Y, unsigned char Color )
{    
  unsigned char Temp ;
    if( X0 > X1 )     // ȷ��С��ַ��ǰ�����ַ�ں󣬷�����н���
    {
       Temp = X1 ;
        X1 = X0 ;
        X0 = Temp ;
    }
    for( ; X0 <= X1 ; X0++ )
    {
      DrawPoint_f( X0, Y, Color ) ;  //���������д���е�ַ����
    }
}
/*******************************************
�������ƣ�DrawLineY_f
��    �ܣ���LCD��ֱ�����ϻ�һ��ֱ��
��    ����X-�кţ�Y0-��ʼ�кţ�
         Y1-��ֹ�кţ� Color--����
��д�ߣ�lh  2013-5-16      
����ֵ����
˵���� ��
********************************************/
void DrawLineY_f( unsigned char X, unsigned char Y0, unsigned char Y1, unsigned char Color )
{
    unsigned char Temp ;
    if( Y0 > Y1 )       // ȷ��С��ַ��ǰ�����ַ�ں󣬷�����н���
    {
        Temp = Y1 ;
        Y1 = Y0 ;
        Y0 = Temp ;
    }
    for(; Y0 <= Y1 ; Y0++)  //����д�У��е�ַ����
    DrawPoint_f( X, Y0, Color);
}
/*******************************************
�������ƣ�circle
��    �ܣ���LCD�ϻ�һ��Բ
��    ����x0��y0ΪԲ�����꣬rΪԲ�İ뾶
��д�ߣ�lh  2013-5-16      
����ֵ����
˵���� ��
********************************************/
void circle(unsigned char x0 , unsigned char y0 , unsigned char r)
{
    int a=0,b,di;
      if(r>31||r==0)   //Բ����Һ��������û�뾶�򷵻�
	return;
	b=r;
	di=3-2*r;     //�ж��¸���λ�õı�־
	while(a <= b)
	{
	DrawPoint_f(x0-b,y0-a,1);  //3
	DrawPoint_f(x0+b,y0-a,1); //0
	DrawPoint_f(x0-a,y0+b,1); //1
	DrawPoint_f(x0-b,y0-a,1); //7
	DrawPoint_f(x0-a,y0-b,1); //2
	DrawPoint_f(x0+b,y0+a,1); //4
	DrawPoint_f(x0+a,y0-b,1); //5
	DrawPoint_f(x0+a,y0+b,1); //6
	DrawPoint_f(x0-b,y0+a,1);
		a ++;
		//***ʹ��bresenham�㷨��Բ********/
		if(di<0)
                  {di+=4*a+6;}
		else
		{
	          di+=10+4*(a-b);
		   b--;
		}	
	DrawPoint_f(x0+a, y0+b,1);
	}
}	
void LcdPos(unsigned char x,unsigned char y)
{
  unsigned char pos;
  if(x==1)x=0x80;
  else if(x==2)x=0x90;
  else if(x==3)x=0x88;
  else if(x==4)x=0x98;
  pos=x+y;
  Write_Cmd(pos);  
}
void LcdWriteString(char line,char row,char *str)
{
    unsigned char addr;
    if(line==1)
      addr=0x80+row;
    else if(line==2)
      addr=0x90+row;
    else if(line==3)
      addr=0x88+row;
    else if(line==4)
      addr=0x98+row;
    Write_Cmd(addr);
    while(*str!='\0')
    {
        Write_Data(*str);
        str++;
    }
}
void Component_Display(uchar i,uchar j,uint x)
{
 uint x1,x2,x3,x4,x5,x6;
 LcdPos(i,j);
 x1=x%1000000/100000+48;
  Write_Data(x1);
 x2=x%100000/10000+48;
  Write_Data(x2);
 x3=x%10000/1000+48;
  Write_Data(x3);
 x4=x%1000/100+48;
  Write_Data(x4);
 x5=x%100/10+48;
  Write_Data(x5);
 x6=x%10+48;
  Write_Data(x6);
}
void Component_Display1(uchar i,uchar j,uchar x)
{
  LcdPos(i,j);
  Write_Data(x);
}
void Component_Display_2(uchar i,uchar j,uchar *a)
{
  LcdPos(i,j);
  Write_Data(a[0]);
  Write_Data(a[1]);
}





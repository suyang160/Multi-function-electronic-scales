#include "12864.h"
/*******************************************
函数名称：Write_Cmd
功    能：向液晶中写控制命令
参    数：cmd--控制命令
返回值  ：无
********************************************/
void Write_Cmd(uchar cmd)
{
    uchar lcdtemp = 0;
			
    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;  
    do                       //判忙
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
函数名称：Write_Data
功    能：向液晶中写显示数据
参    数：dat--显示数据
返回值  ：无
********************************************/
void  Write_Data(uchar dat)
{
    uchar lcdtemp = 0;   
        
    LCD_RS_L;
    LCD_RW_H;  
    LCD_DataIn;   
    do                       //判忙
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
函数名称：Ini_Lcd
功    能：初始化液晶模块
参    数：无
返回值  ：无
********************************************/
void Init_Lcd(void)
{                  
    LCD_CMDOut;    //液晶控制端口设置为输出

    //Delay_Nms(100);
    Write_Cmd(0x30);   //基本指令集
    delay_ms(1);
    Write_Cmd(0x02);   // 地址归位
    delay_ms(1);
	Write_Cmd(0x0c);   //整体显示打开,游标关闭
    delay_ms(1);
	Write_Cmd(0x01);   //清除显示
    delay_ms(1);
	Write_Cmd(0x06);   //游标右移
    delay_ms(1);
	Write_Cmd(0x80);   //设定显示的起始地址
}

/*******************************************
函数名称：Disp_HZ
功    能：控制液晶显示汉字
参    数：addr--显示位置的首地址
          pt--指向显示数据的指针
          num--显示字符个数
返回值  ：无
********************************************/
void Disp_HZ(uchar addr,const char * pt,uchar num)
{
    uchar i;
		
    Write_Cmd(addr); 
   	for(i = 0;i < (num);i++) 
        Write_Data(*(pt++)); 
} 
/*******************************************
函数名称：Disp_3num
功    能：在LCD指定位置写三个数字
参    数：addr--显示位置的首地址
          num--指向显示的三位数
返回值  ：无
********************************************/
void Disp_4num(uchar addr,uint num)
{
    Write_Cmd(addr);
    Write_Data(num%100000/10000+0x30);     //万位    
    Write_Data(num%10000/1000+0x30);       //千位    
    Write_Data(num%1000/100+0x30);         //千位
    Write_Data(num%100/10+0x30);           //十位
    Write_Data(num%10+0x30);               //个位
}

void Disp_3num(uchar addr,int num)
{
  if(num>=0)
  {    
    Write_Cmd(addr-1);
    Write_Data(0x2b);
    Write_Data(num%1000/100+0x30);         //千位
    Write_Data(num%100/10+0x30);           //十位
    Write_Data(num%10+0x30);               //个位
  }
  if(num<0)
  {
    Write_Cmd(addr-1);
    Write_Data(0x2d);
    Write_Data((-num)%1000/100+0x30);         //千位
    Write_Data((-num)%100/10+0x30);           //十位
    Write_Data((-num)%10+0x30);               //个位
  }  
  
}

void Disp_2num(uchar addr,uint num)
{
    Write_Cmd(addr);   
    Write_Data(num%1000/100+0x30);         //千位
    Write_Data(num%100/10+0x30);           //十位
    Write_Data(num%10+0x30);               //个位
}

/*******************************************
函数名称：lcd12864_image
功    能：在LCD上显示整屏图像
参    数：*p--显示内容 64*16字节 
编写者：lh  2013-5-16      
返回值  ：无
说明： 如果是其他 8*8字节图像等注意更改参数
********************************************/
void lcd12864_image(const unsigned char *p)
{
   int x,y;
   Write_Cmd(0x34); //扩展指令集动作,绘图显示OFF
  for (y=0;y<32;y++) //送上半屏显示数据
     {
      Write_Cmd(0x80|y); //送显示Y坐标0x80-0x9f
      Write_Cmd(0x80); //送显示X坐标0x80
   for (x=0;x<16;x++)
        {
          Write_Data(*p);
             p++;
        }
     }
  
for (y=0;y<32;y++) //送下半屏显示数据
   {
    Write_Cmd(0x80|y); //送显示Y坐标0x80-0x9f
    Write_Cmd(0x88); //送显示X坐标0x88
   for (x=0;x<16;x++)
     {
      Write_Data(*p);
       p++;
     }
   }
   Write_Cmd(0x36); //扩展指令集动作,绘图显示ON 注意这里需不需要回到常规指令
    Delay_Nms(50);
   Write_Cmd(0x30); //回到常规指令 
}

/*******************************************
函数名称：gui_clear
功    能：绘图模式下的清屏
参    数：无
编写者：lh  2013-5-16      
返回值  ：无
说明： 绘图模式无法使用Write_Cmd(0x01)清屏
********************************************/
void gui_clear(void)
{
   int x,y;
   Write_Cmd(0x34); //扩展指令集动作,绘图显示OFF
  for (y=0;y<32;y++) //送上半屏显示数据
     {
      Write_Cmd(0x80|y); //送显示Y坐标0x80-0x9f
      Write_Cmd(0x80); //送显示X坐标0x80
   for (x=0;x<16;x++)
        {
          Write_Data(0x00);
        }
     }
  
for (y=0;y<32;y++) //送下半屏显示数据
   {
    Write_Cmd(0x80|y); //送显示Y坐标0x80-0x9f
    Write_Cmd(0x88); //送显示X坐标0x88
   for (x=0;x<16;x++)
     {
      Write_Data(0x00);
     }
   }
   Write_Cmd(0x36); //扩展指令集动作,绘图显示ON 注意这里需不需要回到常规指令
    delay_ms(50);
   Write_Cmd(0x30); //回到常规指令 
}
/*******************************************
函数名称：ReadByte_f
功    能：在LCD上读一个字节亮暗的状态
参    数：无 
编写者：lh  2013-5-16      
返回值  ：一个字节的状态，读DDRAM值，8位
说明： 无  0000 0000 （配合地址使用)，确定某一个字节的数值
********************************************/
unsigned char ReadByte_f( void )
{
    unsigned char byReturnValue ;     // 定义一个无符号的字符型变量为点的状态
    uchar lcdtemp = 0;		
    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;  
    do                       //判忙
    {    
        LCD_EN_H;
       	_NOP();					
       	lcdtemp = LCD2MCU_Data; 
       	LCD_EN_L;   
    }
    while(lcdtemp & 0x80); 
    
    LCD_RS_H;                // RS=1，数据
    LCD_RW_H;                // R/W=1，读命令
    _NOP();		              
    LCD_EN_L;                  //E先低电平，再高电平，上升沿
    LCD_EN_H;
    byReturnValue = LCD2MCU_Data ;   //读数据
    LCD_EN_L;             // 关闭使能
    LCD_CMDOut;      //液晶控制端口设置为输出 
      
    return byReturnValue ;    // 返回读的数据给后续接口用
}
/*****************************************0,0        127,0
函数名称：DrawPoint_f 写DDRAM值            |           |
功    能：在LCD上具体坐标位置画一个点      |           |
参    数：X,Y,Color                        |           |
           X---列号，Y--行号              0,63        127,63
     Color--0 不画点；1--画点；2--异或，点以前亮，现在不亮，反之相反。
编写者：lh  2013-5-16      
返回值：无
说明： Tier为列对应的字节地址，0-16；Tier_bit为列字节里的位，有0-7共8位可能
********************************************/
void DrawPoint_f( unsigned char X, unsigned char Y, unsigned char Color )
{
    unsigned char Row , Tier , Tier_bit    ;  // 定义画点的坐标，X为列0-127，Y为行0-63
    unsigned char ReadOldH, ReadOldL ;      // 定义读回来的状态
    Write_Cmd(0x34) ;         // 常规指令到扩展指令的切换
    	_NOP();
    Write_Cmd(0x36);         // 绘图显示开
    Tier = X >> 4 ;           //右边移动4位，取高位结果，Tier-列对应的字节地址
    Tier_bit = X & 0x0f ;     //Tier_bit为列字节里的位，有0-7共8位可能
    if( Y < 32 )
    {
        Row = Y ;        // 上半屏 80H，行地址
    }
    else
    {
        Row = Y - 32 ;   // 下半屏，行地址
        Tier += 8 ;      //  88H  字地址
    }
    Write_Cmd(Row+0x80) ;   // 行地址
    Write_Cmd(Tier+0x80) ; //  列地址
      ReadByte_f() ;       //读点位置的状态
    ReadOldH = ReadByte_f() ; //读点位置的状态
    ReadOldL = ReadByte_f() ; //读点位置的状态
    Write_Cmd(Row+0x80)    ; //
    Write_Cmd(Tier+0x80) ;  //
    if( Tier_bit < 8 )
    {
        switch( Color)   //1 置位；0 为清0； 2 异或
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
    Write_Cmd(0x30); //回到常规指令   ;
}
/*******************************************
函数名称：DrawLineX_f
功    能：在LCD水平方向上画一条直线
参    数：X0-水平方向起始列号；X1-水平方向终止列号；
         Y-行号； Color--点数
编写者：lh  2013-5-16      
返回值：无
说明： 无
********************************************/
void DrawLineX_f( unsigned char X0, unsigned char X1, unsigned char Y, unsigned char Color )
{    
  unsigned char Temp ;
    if( X0 > X1 )     // 确保小地址在前，大地址在后，否则进行交换
    {
       Temp = X1 ;
        X1 = X0 ;
        X0 = Temp ;
    }
    for( ; X0 <= X1 ; X0++ )
    {
      DrawPoint_f( X0, Y, Color ) ;  //多个点连续写，行地址不变
    }
}
/*******************************************
函数名称：DrawLineY_f
功    能：在LCD垂直方向上画一条直线
参    数：X-列号；Y0-起始行号；
         Y1-终止行号； Color--点数
编写者：lh  2013-5-16      
返回值：无
说明： 无
********************************************/
void DrawLineY_f( unsigned char X, unsigned char Y0, unsigned char Y1, unsigned char Color )
{
    unsigned char Temp ;
    if( Y0 > Y1 )       // 确保小地址在前，大地址在后，否则进行交换
    {
        Temp = Y1 ;
        Y1 = Y0 ;
        Y0 = Temp ;
    }
    for(; Y0 <= Y1 ; Y0++)  //连续写行，列地址不变
    DrawPoint_f( X, Y0, Color);
}
/*******************************************
函数名称：circle
功    能：在LCD上画一个圆
参    数：x0、y0为圆心坐标，r为圆的半径
编写者：lh  2013-5-16      
返回值：无
说明： 无
********************************************/
void circle(unsigned char x0 , unsigned char y0 , unsigned char r)
{
    int a=0,b,di;
      if(r>31||r==0)   //圆大于液晶屏或者没半径则返回
	return;
	b=r;
	di=3-2*r;     //判断下个点位置的标志
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
		//***使用bresenham算法画圆********/
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





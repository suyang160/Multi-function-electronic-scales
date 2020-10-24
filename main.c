//要么不做，要么就做出自己理想的完美状态，加油！！！
//无法容忍数字前一串零
#include  <msp430x14x.h>
#include  <stdlib.h>
#include  "config.h"
#include  "12864.h"
#include  "Keypad.h"
#include  "HX711.h"
#include  "delay.h"
#include  "DS18B20.h"

/***************************************************蜂鸣器定义**************************************/
#define BEEPCMD  P5SEL|=BIT2
#define BEEP_H   P5OUT|=BIT2
#define BEEP_L   P5OUT&=~BIT2
/**************************************************************************************************/
#ifndef uchar
#define uchar unsigned char
#endif

extern unsigned int  A1;
extern unsigned int  A2;                          //<---DS18B20的三个显示位
extern unsigned int  A3;
extern unsigned int    temp_value;
extern unsigned char key_val;         
extern unsigned char key_Flag;
unsigned long HX711_Buffer = 0;
unsigned long  Weight_Maopi = 0,Weight_Shiwu = 0; //<---要用长整型
unsigned long  Weight_Maopi_zancun=0;             

unsigned char table[4][16]={
  {"※※※※※※※※"},
  {"※15公斤电子秤※"},
  {"※※欢迎使用※※"},
  {"※※※※※※※※"},
  };
unsigned char table1[4][16]={
  {"温度:     00.0℃"},                //由于地址递增，实际显示2,3行互换
  {"单价:     0.00元"},
  {"重量:   0.00公斤"},
  {"金额:     0.00元"},
  };

/*********************************************flag标志位的定义************************************/
unsigned char shuru_flag=0;                        
unsigned char leijia_flag=0;                    //这几个标志位只有计数和累加可以叠加存在
unsigned char cunchu_flag=0;
unsigned char qupi_flag=0;
unsigned char zhaoqian_flag=0;
unsigned char jichu_flag=0;
/************************************************************************************************/

unsigned char danjia_array[6]={'0','0','0','.','0','0'};           //最终显示的单价数组
unsigned char danjia_array_yunsuan[5]={'0','0','0','0','0'};       //该数组当上面数组刷新时附值，去小数点，为了计算方便
unsigned char danjia_array_xiugai[6]={'0','0','0','.','0','0'};     //可进行修改的数组，如遇突发状况，回来显示单价数组
                                                //如果正确配置，再将其值拷贝给数组1
                                                //每次准备对其修改之前，都会现将其清零
unsigned char Weight_Shiwu_array[4]={'0','0','0','0'};            //重量数组
uchar bitcount=0 ;                                 //输入的计数

unsigned char recent_price[11]={'0','0','0','0','0','0','0','0','0','0','0'};            //记录当前状态下金额
unsigned char total_price[6]={'0'};            //记录总的金额
unsigned char price_num=0;
unsigned long price1;
//long double price1,price2;                    //price1和price2用来检验在累加状态下称重是否稳定
unsigned long price_end=0.0;                        //如果最终稳定，则将当前价格累计到price_end
                                              //最终对price_end数据进行处理//该些定义均需在累加模式下
unsigned above_chen=0;                        //用来表示目前是否有重物置于秤之上。。。
unsigned char  price_temp;                    //该状态下临时存储一数值的变量。

uchar tree[5]={'1','0','0','0','0'};          //记录状态树，用其替代掉之前的flag，树深5
uchar M[11][5]={'0'};                         //暂时不能掉电保存
uchar adress=4;                                 //用 adress标志地址值 adress初始为4
uchar adress_flag=0;                            //用adress标志该地址处属于前半段还是后半段
uchar point_flag=0;                             //没按小数点改值为0，按下小数点该值为1
uchar fen_flag=0;                               //小数点后分位
uchar baifen_flag=0;
uchar count;
uchar leijia_count=0;                             //按了多少次累加键计数
void Get_Weight();
void Get_Maopi();
void multiply(unsigned char* a,unsigned char* b,unsigned char* c);//大数乘法
void show_price(unsigned char i);
void price_value(unsigned char i);

//暂时先已20公斤做为上限
void main()
{
    WDT_Init();
    Clock_Init();
    Init_Lcd();
    Init_Keypad();
    DS18B20_Reset();			  //复位D18B20
    uchar i,j;
//开机画面显示
    unsigned long a;//试验字母
    Write_Cmd(0x80);
    for(i=0;i<4;i++)
    {
      for(j=0;j<16;j++)
      {
       Write_Data(table[i][j]);
      }
    }
    delay_ms(500);
     
//称重状态显示
    Write_Cmd(0x80);
    for(i=0;i<4;i++)
    {
      for(j=0;j<16;j++)
      {
       Write_Data(table1[i][j]);
      }
    }
    jichu_flag=1;
    uchar value;                                   //value存了0~9键值，当为10时为小数点
    Get_Maopi();				//称毛皮重量
    Write_Cmd(0x92);                            //等待传感器稳定期间标志位
    Write_Data(':');
    Write_Data('W');    
    delay_ms(5000);		 //延时5s,等待传感器稳定
    Write_Cmd(0x92);                            //传感器稳定,标志位解除
    Write_Data(':');
    Write_Data(' ');        
    Get_Maopi();
  while(1)
   {
       Get_Weight();//称重
     //显示当前重量
     Write_Cmd(0x93);
     Write_Data(' ');
     if(Weight_Shiwu%10>=5)
     {
     Weight_Shiwu=Weight_Shiwu/10+1;
     }
     else
     {
      Weight_Shiwu=Weight_Shiwu/10; 
     }
/*     if(leijia_flag==1)       //如果在累加模式下，则进入做特殊处理，判断当前是否有重物
     {                        //并且延时，使传感器达到最终检测状态
       if(Weight_Shiwu==0)
       {
         delay_ms(500);
         if(Weight_Shiwu==0)
         {
           above_chen=0;     //表示此时无重物
         }
       }
       else
       {
         delay_ms(500);
         if(above_chen==0)
         {
           above_chen=1;
           price_temp=1;   //<----------非常关键的标志位，它为1，则累加一次，不为1，则不加
         }         
       }              
     }*/
     if(Weight_Shiwu<=2000)
     {
       if( Weight_Shiwu/1000 != 0)
       {
          Write_Data(Weight_Shiwu/1000 + 0x30);            //并将当前值显示，此时weight_shiwu为4位数
       }
       else
       {
          Write_Data(' ');	
       }
       Write_Data(Weight_Shiwu/100%10 + 0x30);
       Write_Data('.');
       Write_Data(Weight_Shiwu/10%10 + 0x30);
       Write_Data(Weight_Shiwu%10 + 0x30);
       Weight_Shiwu_array[0]=Weight_Shiwu/1000 + 0x30;    
       Weight_Shiwu_array[1]=Weight_Shiwu/100%10 + 0x30;
       Weight_Shiwu_array[2]=Weight_Shiwu/10%10 + 0x30;
       Weight_Shiwu_array[3]=Weight_Shiwu%10 + 0x30;
     }
     else
     {
        Write_Data(' ');
        Write_Data('-');                                       //当重量超过20后输出-.--
        Write_Data('.');
        Write_Data('-');
        Write_Data('-');
     }
      
    //显示当前温度
    ds1820_start();		          //启动一次转换
    ds1820_read_temp();		          //读取温度数值
    data_do(temp_value);                  //处理数据，得到要显示的值
    Write_Cmd(0x85);
    Write_Data(A1+0x30);
    Write_Data(A2+0x30);
    Write_Data('.');
    Write_Data(A3+0x30);
    delay_ms(1);
       if(key_falg())
    {
      Check_Key();
      if(key_val==27)            //按下输入键，进入输入状态
      {//<----(27输入)
         Write_Cmd(0x8a);
         Write_Data(':');
         Write_Data('P');        //输入状态标志
         Write_Cmd(0x8c);
         Write_Data(' ');
         Write_Data(' ');
         Write_Data('0');
         Write_Data('.');       //输入状态下初显为0.00
         Write_Data('0');
         Write_Data('0');         
         count=0;point_flag=0;  fen_flag=0;  baifen_flag=0;//各标志位清0
      loop:    while(key_falg()==0);     //等待下一按键输入
      
          if(key_falg())
          {
           Check_Key();
           while(key_val!=27)//直到按下输入键退出
           {//<---输入键括号
    /////////////////////////////////按下数字键后所做的相应调整//////////////////////////////////////////
             if(key_val<=3||(key_val>=7&&key_val<=9)|| (key_val>=13&&key_val<=15)||(key_val>=19&&key_val<=20))//为数字键
             {
               if(key_val<=3)
               {value=key_val+6;}
               else if(key_val>=7&&key_val<=9)
               {value=key_val-3;}
               else if(key_val>=13&&key_val<=15)                          //得到键所对应的数字值
               {value=key_val-12;}
               else if(key_val==19)
               {value=0;}
               else
               {value=10;}                                      //代表小数点       
               if(baifen_flag==1)                      //<------当输到底后再按数字键无反应
               {}
               else
               {
                     if(value<10)
                     {
                         if(point_flag==0)
                         {
                             count++;                        //根据count值枚举
                             if(count==1)                   //00 0. 00
                             {
                              Write_Cmd(0x8D);
                              Write_Data(value+0x30);
                              danjia_array_xiugai[2]=value+0x30;
                             }
                             else if(count==2)
                             {
                              Write_Cmd(0x8c);
                              Write_Data(' ');
                              Write_Data(danjia_array_xiugai[2]);
                              Write_Data(value+0x30);
                              danjia_array_xiugai[1]=danjia_array_xiugai[2];
                              danjia_array_xiugai[2]=value+0x30;
                             }
                             else if(count==3)
                             {
                               Write_Cmd(0x8c);
                               Write_Data(danjia_array_xiugai[1]);
                               Write_Data(danjia_array_xiugai[2]);
                               Write_Data(value+0x30);
                               danjia_array_xiugai[0]=danjia_array_xiugai[1];
                               danjia_array_xiugai[1]=danjia_array_xiugai[2];
                               danjia_array_xiugai[2]=value+0x30;
                             }
                             else
                             {
                               count=5;                 //没有按下'.'也认为按了
                               Write_Cmd(0x8e);
                               Write_Data(value+0x30);
                               danjia_array_xiugai[3]='.';
                               danjia_array_xiugai[4]=value+0x30;
                               point_flag=1;
                               fen_flag=1;
                               goto loop;
                             }                       
                         }
                         if(point_flag==1)
                         {
                           if(fen_flag==0)
                           {
                           count=5;
                           Write_Cmd(0x8e);
                           Write_Data(value+0x30);
                           danjia_array_xiugai[4]=value+0x30;          //可能有些标志位重复
                           fen_flag=1;
                           }
                           else
                           {
                             count=6;
                             Write_Cmd(0x8e);
                             Write_Data(danjia_array_xiugai[4]);                   
                             Write_Data(value+0x30);
                             danjia_array_xiugai[5]=value+0x30;
                             baifen_flag=1;
                           }
                         }
                     }
                           if(value==10)
                         {
                         danjia_array_xiugai[3]='.';
                         point_flag=1;
                         count=4;
                         } 
                 }
             }//<-------数字键括号
             goto loop;     
      /////////////////////////////////////////////////////////////////////////////////////////////////////             
            } //<------输入键括号
           if(key_val==27)//<----再次按下输入键//直到再次按下输入键程序往后跑
           {
               Write_Cmd(0x8a);
               Write_Data(':');
               Write_Data(' ');        //清除输入状态标志
             count=0;point_flag=0;  fen_flag=0;  baifen_flag=0;//与之相关的标志位清0
             for(i=0;i<6;i++)
             {
               danjia_array[i]=danjia_array_xiugai[i];    /////完成待输入价格的确认 ，数组的复制 
               danjia_array_xiugai[i]='0';                //并将之前数组清零
               if(i==3)
               {danjia_array_xiugai[i]='.';} 
             }
             danjia_array_yunsuan[0]=danjia_array[0];
             danjia_array_yunsuan[1]=danjia_array[1];
             danjia_array_yunsuan[2]=danjia_array[2];    //danjia_array_yunsuan数据刷新，纯枚举
             danjia_array_yunsuan[3]=danjia_array[4];
             danjia_array_yunsuan[4]=danjia_array[5];
               if( danjia_array[0]=='0') //<----对单价刷新，将数字前面的'0'用' '显示 譬如09.10—9.10
               {
                 Write_Cmd(0x8c);
                 Write_Data(' ');
                 if(danjia_array[1]=='0')
                  Write_Data(' ');                
               }

           }
           
          }
       }//<----(27输入)
       if(key_val==16)            //按下去皮键，进入去皮状态   标志M
      {                           //检测到第二次按下，则去皮状态消失
        if(qupi_flag==0)          //放上东西后再按这个键
        {
          Write_Cmd(0x92);
          Write_Data(':');
          Write_Data('M');
          Weight_Maopi_zancun=Weight_Maopi;
          Get_Maopi();
          qupi_flag=1;
        }
        else
        {
          qupi_flag=0;
          Write_Cmd(0x92);
          Write_Data(':');
          Write_Data(' ');
          Weight_Maopi=Weight_Maopi_zancun;
        }        
      }//<----（16按键）
      if(key_val==10)//累加键标志符A
      {
       
          leijia_flag=1;
          leijia_count++;
          Write_Cmd(0x82);
          Write_Data(':');
          Write_Data('A');    //<----累加模式应只有置零键可退出
          if(leijia_count<10)
          {
          Write_Data(leijia_count+0x30);
          }
          else if(leijia_count>=10&&leijia_count<=99)
          {
          Write_Data(leijia_count/10+0x30);
          Write_Data(leijia_count%10+0x30);//<---更新累计次数，最大999次
          }
          else
          {
          Write_Data(leijia_count/100+0x30);
          Write_Data(leijia_count/10%10+0x30);//<---更新累计次数，最大999次,1000次后显示异常
          Write_Data(leijia_count%10+0x30);//<---更新累计次数，最大999次  
          }
          multiply(danjia_array_yunsuan,Weight_Shiwu_array,recent_price);
            for(i=0;i<11;i++)
           {
             if(recent_price[i]=='\0')
             {
               break;
             }
           }//<-----------此时i表示共有几位数字，根据i的大小进行判断
            price_num=i;           
            price_value(price_num);
      }//<----(10累加键)
      if(key_val==22)//置零键
      {
       leijia_flag=0;
       price_end=0;
       Write_Cmd(0x82);
       Write_Data(':');
       Write_Data(' ');    //<----累加模式应只有置零键可退出
       Write_Data(' ');
       Write_Data(' ');
       Write_Data(' ');    //<----累加次数清零       
      }//<----(22置零键)
    
    }//<---判断是否有按键按下
    
     //单价与重量相乘算法无论位数多高，貌似msp430有乘法器，可不会，下次学。
     
     Write_Cmd(0x9a);
     Write_Data(':');
     for(i=0;i<5;i++)
     {Write_Data(' ');} 
     Write_Data('0');
     Write_Data('.');
     Write_Data('0');
     Write_Data('0');                    //再与改变价格前，先将之前数字刷新
     multiply(danjia_array_yunsuan,Weight_Shiwu_array,recent_price);
      for(i=0;i<11;i++)
     {
       if(recent_price[i]=='\0')
       {
         break;
       }
     }//<-----------此时i表示共有几位数字，根据i的大小进行判断
      price_num=i;
    show_price(price_num);
     
 }//<---while大循环括号
}
void price_value(unsigned char i)//<--算出当前值，并且更新price_end
{  
        if(i>8)
       {
        price_end=10000;
       }
       else if(i==8)                                      //<-------------愚蠢的我只想枚举
       {
         price_end+=(recent_price[0]-0x30)*100000;
         price_end+=(recent_price[1]-0x30)*10000;
         price_end+=(recent_price[2]-0x30)*1000;
         price_end+=(recent_price[3]-0x30)*100;
         price_end+=(recent_price[4]-0x30)*10;
         price_end+=(recent_price[5]-0x30);
       }
       else if(i==7)                                     
       {
         price_end+=(recent_price[0]-0x30)*10000;
         price_end+=(recent_price[1]-0x30)*1000;
         price_end+=(recent_price[2]-0x30)*100;
         price_end+=(recent_price[3]-0x30)*10;
         price_end+=(recent_price[4]-0x30);
       }
       else if(i==6)                                      
       {
         price_end+=(recent_price[0]-0x30)*1000;
         price_end+=(recent_price[1]-0x30)*100;
         price_end+=(recent_price[2]-0x30)*10;
         price_end+=(recent_price[3]-0x30);   
       }
       else if(i==5)
       {
         price_end+=(recent_price[0]-0x30)*100;
         price_end+=(recent_price[1]-0x30)*10;
         price_end+=(recent_price[2]-0x30);
       }
        else if(i==4)
       {
         price_end+=(recent_price[0]-0x30)*10;
         price_end+=(recent_price[1]-0x30);
       }
        else if(i==3)
       {
         price_end+=(recent_price[1]-0x30); 
       }
       else if(i==2)
       {
       }
       else{}
}

//************************************************************************************
void show_price(unsigned char i)            //针对大数乘法所得的位数于显示屏上得到相应显示
{
   if(leijia_flag==0)
   {
        if(i>8)
       {
         Write_Cmd(0x9d);
         Write_Data('-');
         Write_Data('.');
         Write_Data('-');
         Write_Data('-');       
       }
       else if(i==8)                                      //<-------------愚蠢的我只想枚举
       {
         Write_Cmd(0x9b);
         Write_Data(' ');
         Write_Data(recent_price[0]);
         Write_Data(recent_price[1]);
         Write_Data(recent_price[2]);
         Write_Data(recent_price[3]);
         Write_Data('.');
         Write_Data(recent_price[4]);
         Write_Data(recent_price[5]);
       }
       else if(i==7)                                      //<-------------愚蠢的我只想枚举
       {
         Write_Cmd(0x9c);
         Write_Data(recent_price[0]);
         Write_Data(recent_price[1]);
         Write_Data(recent_price[2]);
         Write_Data('.');
         Write_Data(recent_price[3]);
         Write_Data(recent_price[4]);
       }
       else if(i==6)                                      
       {
         Write_Cmd(0x9c);
         Write_Data(' ');
         Write_Data(recent_price[0]);
         Write_Data(recent_price[1]);
         Write_Data('.');
         Write_Data(recent_price[2]);
         Write_Data(recent_price[3]);
       }
       else if(i==5)
       {
         Write_Cmd(0x9d);
         Write_Data(recent_price[0]);
         Write_Data('.');
         Write_Data(recent_price[1]);
         Write_Data(recent_price[2]); 
       }
        else if(i==4)
       {
         Write_Cmd(0x9d);
         Write_Data('0');
         Write_Data('.');
         Write_Data(recent_price[0]);
         Write_Data(recent_price[1]); 
       }
        else if(i==3)
       {
         Write_Cmd(0x9d);
         Write_Data('0');
         Write_Data('.');
         Write_Data('0');
         Write_Data(recent_price[0]); 
       }
       else if(i==2)
       {
         Write_Cmd(0x9d);
         Write_Data('0');
         Write_Data('.');
         Write_Data('0');
         Write_Data('0'); 
       }
   }
   else
   {//下面在累加状态下的特殊情况    
         if(price_end>=1000000)     //如果累计金额大于9999.99则异常显示
       {
         Write_Cmd(0x9d);
         Write_Data('-');
         Write_Data('.');
         Write_Data('-');
         Write_Data('-');          
       }
       else
       {
            Write_Cmd(0x9b);
            Write_Data(' ');
            Write_Data(price_end/100000 + 0x30);
            Write_Data(price_end%100000/10000 + 0x30);
            Write_Data(price_end%10000/1000 + 0x30);
            Write_Data(price_end%1000/100 + 0x30);
            Write_Data('.');
            Write_Data(price_end%100/10 + 0x30);
            Write_Data(price_end%10 + 0x30);
            if(price_end/100000 + 0x30=='0')
            {
            Write_Cmd(0x9b);
            Write_Data(' ');
            Write_Data(' ');
              if(price_end%100000/10000 + 0x30=='0')
              {
                Write_Data(' ');
                if(price_end%10000/1000 + 0x30=='0')
                {
                 Write_Data(' ');                 
                }
              }
            }
       }
   }
}
//************************************************************************************
void multiply(unsigned char* a,unsigned char* b,unsigned char* c)//单价的数组5位*重量4位=10位给c数组11位
{
    int i,j,ca,cb,*s;
    ca=5;
    cb=4;
    s=(int*)malloc(sizeof(int)*(ca+cb));
    for (i=0;i<ca+cb;i++)
        s[i]=0;
    for (i=0;i<ca;i++)
        for (j=0;j<cb;j++)
            s[i+j+1]+=(a[i]-'0')*(b[j]-'0');
    for (i=ca+cb-1;i>=0;i--)
        if (s[i]>=10)
        {
            s[i-1]+=s[i]/10;
            s[i]%=10;
        }
    i=0;
    while (s[i]==0)
        i++;
       for (j=0;i<ca+cb;i++,j++)
           c[j]=s[i]+'0';
    c[j]='\0';
    free(s);
}

//************************************************************************************
  void Get_Weight()
{
	 HX711_Buffer = HX711_Read();
	 HX711_Buffer = HX711_Buffer/100;
/*       Write_Cmd(0x90);
         Write_Data(HX711_Buffer/1000000 + 0x30);
         Write_Data(HX711_Buffer%1000000/100000 + 0x30);
         Write_Data(HX711_Buffer%100000/10000 + 0x30);
         Write_Data(HX711_Buffer%10000/1000 + 0x30);
         Write_Data(HX711_Buffer%1000/100 + 0x30);
         Write_Data(HX711_Buffer%100/10 + 0x30);   //----->调试代码，由于第一次测好删了后又有问题
         Write_Data(HX711_Buffer%10 + 0x30);       //----->又重写，所以绝对不删
         Write_Cmd(0x98);
         Write_Data(Weight_Maopi/100000 + 0x30);
         Write_Data(HX711_Buffer%100000/10000 + 0x30);
         Write_Data(Weight_Maopi%10000/1000 + 0x30);
         Write_Data(Weight_Maopi%1000/100 + 0x30);
         Write_Data(Weight_Maopi%100/10 + 0x30);
         Write_Data(Weight_Maopi%10 + 0x30);*/
	if(HX711_Buffer >= Weight_Maopi)			
	{
		Weight_Shiwu = HX711_Buffer;
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi;	//获取实物的AD采样数值。
	
	        Weight_Shiwu = (unsigned int)((float)Weight_Shiwu/2.10+0.05);           
             //计算实物的实际重量
             //因为不同的传感器特性曲线不一样，因此，每一个传感器需要矫正这里的2.15这个除数。
             //当发现测试出来的重量偏大时，增加该数值。
             //如果测试出来的重量偏小时，减小改数值。
             //该数值一般在2.15附近调整之间。因传感器不同而定。
             //+0.05是为了四舍五入百分位
		BEEPCMD;
		if(  Weight_Shiwu > 20000 )
		{
			BEEP_L;				//打开警报	
		}
		else
		{
			BEEP_H;			        //关闭警报
		}
	}
	else if(HX711_Buffer < Weight_Maopi - 30) 
	{
		   	BEEP_L;		               //负重量报警
	}

}
//****************************************************
//获取毛皮重量
//****************************************************
  void Get_Maopi()
{
	HX711_Buffer = HX711_Read();
	Weight_Maopi = HX711_Buffer/100;	 			
}

/*///////////////////////树操作//////////////////////////////
 void set_tree(uchar i)
{
   int j;
   for(j=0;j<6;j++)
   {
     if(tree[j]=='0')
     {
       tree[j]=i;
       return;
     }
   }
}

uchar return_tree()//返回树状态
{
  int j,i;
  for(j=0;j<6;j++)              //树的第一位永远为1
   {
     if(tree[j]=='0')
     {
       if(tree[j-1]=='5')       //去皮
       {
         for(i=0;i<=j-2;i++)
         {
           if(tree[i]=='6')
              return '7';
         }
         return tree[j-1];
       }
       else if(tree[j-1]=='6')        //累计
       {
         for(i=0;i<=j-2;i++)        
         {
           if(tree[i]=='5') 
              return '7';              //返回7代表累加与去皮功能同时共存
         }                           //有空再优化，绝对可优化。。。
         return tree[j-1];
       }
       else
       {return tree[j-1];}
     }
   }
}

void clear_tree()
{
  int j;
  tree[0]='1';
  for(j=1;j<5;j++)
  {
   tree[j]='0'; 
  }
}


/////////////////////////////////键盘扫描状态修改数组修改程序///////////////////////////////////////
 void change_state()
{
   Key_Event();
   uchar value;                                   //value存了0~9键值，当为10时为小数点
   uchar state;
   if(key_Flag == 1)
   {
     key_Flag=0;
     if(key_val<=3||7<=key_val<=9|| 13<=key_val<=15||19<=key_val<=20)//为数字键
     {
       if(key_val<=3)
       {value=key_val+6;}
       else if(7<=key_val<=9)
       {value=key_val-3;}
       else if(13<=key_val<=15)
       {value=key_val-12;}
       else if(key_val==19)
       {value=key_val-19;}
       else
       {value=10;}                                      //代表小数点
       
       state=return_tree();
       if(state=='2')
       shuru_cunchu_change(value);                            
       if(state=='3')
       shuru_cunchu_change(value);    
     }
      if(key_val==4)
        qingchu();
      if(key_val==27)//输入键
      {
        if(return_tree()=='2')
        {
         copy_danjia_array();  
        }
        set_tree('2');
      }
      if(key_val==21)//存储键
        set_tree('3');
      if(key_val==28)//找钱键
        set_tree('4');
      if(key_val==16)//去皮键
        set_tree('5');
      if(key_val==10)//累计键
        set_tree('6');
      if(key_val==10)//置零键
        clear_tree();
      M_jian();

        
   }
     
}


//////////////////////////////////////////各状态显示程序//////////////////////////////////////////
   void copy_danjia_array()            //将danjia_array_xiugai[6]中的值拷贝到danjia_array[6]中
   {                                   //当danjia_array重录后刷新显示
     int i;
     for(i=0;i<6;i++)
     {
      danjia_array[i]=danjia_array_xiugai[i]; 
     }
    show(danjia_array);     
   }
  void shuru_cunchu_change(uchar value)                //该状态下须严格按照XXX.XX格式输入
  {
    int i;
    if(adress<=6)                                     //如若地址大于6到了元处，输入无效
    {
        if(adress==4&&adress_flag==0)                 //首次输入，danjia_array_xiugai[5]清零
        {
          for(i=0;i<6;i++)
          {
            danjia_array_xiugai[i]='0';               
          }
          danjia_array_xiugai[i]='.';
          show(danjia_array_xiugai);          
        }
        if(adress==5&&adress_flag==1)                //到了小数点处特殊处理
        {
          adress++;adress_flag=0
        }
        else
        {
          Write_Cmd(0x88+adress);                       //刚开始按下输入键，adress设为初始值4
          if(adress_flag==0)                            //adress_flag设为初始值0
          {
            Write_Data(value+0x30);
            danjia_array_xiugai[adress-4+adress_flag]=value+0x30;
            adress_flag=1;
          }
          else
          {
            Write_Data(' ');
            Write_Data(value+0x30);
            danjia_array_xiugai[adress-4+adress_flag]=value+0x30;
            adress_flag=0;
            adress++;
          } 
        }
    }    
  }
  void qingchu()                                   //该函数暂先预留
  {
    
  }
  
  void M_jian()
{
  
    if(key_val==5)     copy(1);
    if(key_val==11)    copy(2);
    if(key_val==17)    copy(3);
    if(key_val==23)    copy(4);
    if(key_val==29)    copy(5);
    if(key_val==25)    copy(6);
    if(key_val==26)    copy(7);
    if(key_val==6)     copy(8);
    if(key_val==12)    copy(9);
    if(key_val==18)    copy(10);
    show(danjia_array);
}
 void copy(unsigned char row)
 {
   danjia_array[row][0]=M[row][0];
   danjia_array[row][1]=M[row][1];
   danjia_array[row][2]=M[row][2];
   danjia_array[row][3]=M[row][3];
   danjia_array[row][4]=M[row][4];
 }
 void show(uchar *s)          //最终于显示屏上显示的单价金额
 {
   Write_Cmd(0x8c);
   for(i=0;i<6;i++)
   {
     Write_Data(*(s+i));
   }
 }

////////////////////////////AT24C16值调用////////////////////////////////////////////////////

void AT24C16()
{
  if(key_val==10)
}




//************************************************************************************
  void Get_Weight()
{
	HX711_Buffer = HX711_Read();
	HX711_Buffer = HX711_Buffer/100;
	if(HX711_Buffer >= Weight_Maopi)			
	{
		Weight_Shiwu = HX711_Buffer;
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi;	//获取实物的AD采样数值。
	
		Weight_Shiwu = (unsigned int)((float)Weight_Shiwu/1.94+0.05);           
             //计算实物的实际重量
             //因为不同的传感器特性曲线不一样，因此，每一个传感器需要矫正这里的2.15这个除数。
             //当发现测试出来的重量偏大时，增加该数值。
             //如果测试出来的重量偏小时，减小改数值。
             //该数值一般在2.15附近调整之间。因传感器不同而定。
             //+0.05是为了四舍五入百分位
		BEEPCMD;
		if(  Weight_Shiwu > 20000 )
		{
			BEEP_L;				//打开警报	
		}
		else
		{
			BEEP_H;			        //关闭警报
		}
	}
	else if(HX711_Buffer < Weight_Maopi - 30)
	{
		   	BEEP_L;		               //负重量报警
	}

}
//****************************************************
//获取毛皮重量
//****************************************************
  void Get_Maopi()
{
	HX711_Buffer = HX711_Read();
	Weight_Maopi = HX711_Buffer/100;	 			
}

*/

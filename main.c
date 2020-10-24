//Ҫô������Ҫô�������Լ����������״̬�����ͣ�����
//�޷���������ǰһ����
#include  <msp430x14x.h>
#include  <stdlib.h>
#include  "config.h"
#include  "12864.h"
#include  "Keypad.h"
#include  "HX711.h"
#include  "delay.h"
#include  "DS18B20.h"

/***************************************************����������**************************************/
#define BEEPCMD  P5SEL|=BIT2
#define BEEP_H   P5OUT|=BIT2
#define BEEP_L   P5OUT&=~BIT2
/**************************************************************************************************/
#ifndef uchar
#define uchar unsigned char
#endif

extern unsigned int  A1;
extern unsigned int  A2;                          //<---DS18B20��������ʾλ
extern unsigned int  A3;
extern unsigned int    temp_value;
extern unsigned char key_val;         
extern unsigned char key_Flag;
unsigned long HX711_Buffer = 0;
unsigned long  Weight_Maopi = 0,Weight_Shiwu = 0; //<---Ҫ�ó�����
unsigned long  Weight_Maopi_zancun=0;             

unsigned char table[4][16]={
  {"����������������"},
  {"��15������ӳӡ�"},
  {"������ӭʹ�á���"},
  {"����������������"},
  };
unsigned char table1[4][16]={
  {"�¶�:     00.0��"},                //���ڵ�ַ������ʵ����ʾ2,3�л���
  {"����:     0.00Ԫ"},
  {"����:   0.00����"},
  {"���:     0.00Ԫ"},
  };

/*********************************************flag��־λ�Ķ���************************************/
unsigned char shuru_flag=0;                        
unsigned char leijia_flag=0;                    //�⼸����־λֻ�м������ۼӿ��Ե��Ӵ���
unsigned char cunchu_flag=0;
unsigned char qupi_flag=0;
unsigned char zhaoqian_flag=0;
unsigned char jichu_flag=0;
/************************************************************************************************/

unsigned char danjia_array[6]={'0','0','0','.','0','0'};           //������ʾ�ĵ�������
unsigned char danjia_array_yunsuan[5]={'0','0','0','0','0'};       //�����鵱��������ˢ��ʱ��ֵ��ȥС���㣬Ϊ�˼��㷽��
unsigned char danjia_array_xiugai[6]={'0','0','0','.','0','0'};     //�ɽ����޸ĵ����飬����ͻ��״����������ʾ��������
                                                //�����ȷ���ã��ٽ���ֵ����������1
                                                //ÿ��׼�������޸�֮ǰ�������ֽ�������
unsigned char Weight_Shiwu_array[4]={'0','0','0','0'};            //��������
uchar bitcount=0 ;                                 //����ļ���

unsigned char recent_price[11]={'0','0','0','0','0','0','0','0','0','0','0'};            //��¼��ǰ״̬�½��
unsigned char total_price[6]={'0'};            //��¼�ܵĽ��
unsigned char price_num=0;
unsigned long price1;
//long double price1,price2;                    //price1��price2�����������ۼ�״̬�³����Ƿ��ȶ�
unsigned long price_end=0.0;                        //��������ȶ����򽫵�ǰ�۸��ۼƵ�price_end
                                              //���ն�price_end���ݽ��д���//��Щ����������ۼ�ģʽ��
unsigned above_chen=0;                        //������ʾĿǰ�Ƿ����������ڳ�֮�ϡ�����
unsigned char  price_temp;                    //��״̬����ʱ�洢һ��ֵ�ı�����

uchar tree[5]={'1','0','0','0','0'};          //��¼״̬�������������֮ǰ��flag������5
uchar M[11][5]={'0'};                         //��ʱ���ܵ��籣��
uchar adress=4;                                 //�� adress��־��ֵַ adress��ʼΪ4
uchar adress_flag=0;                            //��adress��־�õ�ַ������ǰ��λ��Ǻ���
uchar point_flag=0;                             //û��С�����ֵΪ0������С�����ֵΪ1
uchar fen_flag=0;                               //С������λ
uchar baifen_flag=0;
uchar count;
uchar leijia_count=0;                             //���˶��ٴ��ۼӼ�����
void Get_Weight();
void Get_Maopi();
void multiply(unsigned char* a,unsigned char* b,unsigned char* c);//�����˷�
void show_price(unsigned char i);
void price_value(unsigned char i);

//��ʱ����20������Ϊ����
void main()
{
    WDT_Init();
    Clock_Init();
    Init_Lcd();
    Init_Keypad();
    DS18B20_Reset();			  //��λD18B20
    uchar i,j;
//����������ʾ
    unsigned long a;//������ĸ
    Write_Cmd(0x80);
    for(i=0;i<4;i++)
    {
      for(j=0;j<16;j++)
      {
       Write_Data(table[i][j]);
      }
    }
    delay_ms(500);
     
//����״̬��ʾ
    Write_Cmd(0x80);
    for(i=0;i<4;i++)
    {
      for(j=0;j<16;j++)
      {
       Write_Data(table1[i][j]);
      }
    }
    jichu_flag=1;
    uchar value;                                   //value����0~9��ֵ����Ϊ10ʱΪС����
    Get_Maopi();				//��ëƤ����
    Write_Cmd(0x92);                            //�ȴ��������ȶ��ڼ��־λ
    Write_Data(':');
    Write_Data('W');    
    delay_ms(5000);		 //��ʱ5s,�ȴ��������ȶ�
    Write_Cmd(0x92);                            //�������ȶ�,��־λ���
    Write_Data(':');
    Write_Data(' ');        
    Get_Maopi();
  while(1)
   {
       Get_Weight();//����
     //��ʾ��ǰ����
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
/*     if(leijia_flag==1)       //������ۼ�ģʽ�£�����������⴦���жϵ�ǰ�Ƿ�������
     {                        //������ʱ��ʹ�������ﵽ���ռ��״̬
       if(Weight_Shiwu==0)
       {
         delay_ms(500);
         if(Weight_Shiwu==0)
         {
           above_chen=0;     //��ʾ��ʱ������
         }
       }
       else
       {
         delay_ms(500);
         if(above_chen==0)
         {
           above_chen=1;
           price_temp=1;   //<----------�ǳ��ؼ��ı�־λ����Ϊ1�����ۼ�һ�Σ���Ϊ1���򲻼�
         }         
       }              
     }*/
     if(Weight_Shiwu<=2000)
     {
       if( Weight_Shiwu/1000 != 0)
       {
          Write_Data(Weight_Shiwu/1000 + 0x30);            //������ǰֵ��ʾ����ʱweight_shiwuΪ4λ��
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
        Write_Data('-');                                       //����������20�����-.--
        Write_Data('.');
        Write_Data('-');
        Write_Data('-');
     }
      
    //��ʾ��ǰ�¶�
    ds1820_start();		          //����һ��ת��
    ds1820_read_temp();		          //��ȡ�¶���ֵ
    data_do(temp_value);                  //�������ݣ��õ�Ҫ��ʾ��ֵ
    Write_Cmd(0x85);
    Write_Data(A1+0x30);
    Write_Data(A2+0x30);
    Write_Data('.');
    Write_Data(A3+0x30);
    delay_ms(1);
       if(key_falg())
    {
      Check_Key();
      if(key_val==27)            //�������������������״̬
      {//<----(27����)
         Write_Cmd(0x8a);
         Write_Data(':');
         Write_Data('P');        //����״̬��־
         Write_Cmd(0x8c);
         Write_Data(' ');
         Write_Data(' ');
         Write_Data('0');
         Write_Data('.');       //����״̬�³���Ϊ0.00
         Write_Data('0');
         Write_Data('0');         
         count=0;point_flag=0;  fen_flag=0;  baifen_flag=0;//����־λ��0
      loop:    while(key_falg()==0);     //�ȴ���һ��������
      
          if(key_falg())
          {
           Check_Key();
           while(key_val!=27)//ֱ������������˳�
           {//<---���������
    /////////////////////////////////�������ּ�����������Ӧ����//////////////////////////////////////////
             if(key_val<=3||(key_val>=7&&key_val<=9)|| (key_val>=13&&key_val<=15)||(key_val>=19&&key_val<=20))//Ϊ���ּ�
             {
               if(key_val<=3)
               {value=key_val+6;}
               else if(key_val>=7&&key_val<=9)
               {value=key_val-3;}
               else if(key_val>=13&&key_val<=15)                          //�õ�������Ӧ������ֵ
               {value=key_val-12;}
               else if(key_val==19)
               {value=0;}
               else
               {value=10;}                                      //����С����       
               if(baifen_flag==1)                      //<------���䵽�׺��ٰ����ּ��޷�Ӧ
               {}
               else
               {
                     if(value<10)
                     {
                         if(point_flag==0)
                         {
                             count++;                        //����countֵö��
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
                               count=5;                 //û�а���'.'Ҳ��Ϊ����
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
                           danjia_array_xiugai[4]=value+0x30;          //������Щ��־λ�ظ�
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
             }//<-------���ּ�����
             goto loop;     
      /////////////////////////////////////////////////////////////////////////////////////////////////////             
            } //<------���������
           if(key_val==27)//<----�ٴΰ��������//ֱ���ٴΰ������������������
           {
               Write_Cmd(0x8a);
               Write_Data(':');
               Write_Data(' ');        //�������״̬��־
             count=0;point_flag=0;  fen_flag=0;  baifen_flag=0;//��֮��صı�־λ��0
             for(i=0;i<6;i++)
             {
               danjia_array[i]=danjia_array_xiugai[i];    /////��ɴ�����۸��ȷ�� ������ĸ��� 
               danjia_array_xiugai[i]='0';                //����֮ǰ��������
               if(i==3)
               {danjia_array_xiugai[i]='.';} 
             }
             danjia_array_yunsuan[0]=danjia_array[0];
             danjia_array_yunsuan[1]=danjia_array[1];
             danjia_array_yunsuan[2]=danjia_array[2];    //danjia_array_yunsuan����ˢ�£���ö��
             danjia_array_yunsuan[3]=danjia_array[4];
             danjia_array_yunsuan[4]=danjia_array[5];
               if( danjia_array[0]=='0') //<----�Ե���ˢ�£�������ǰ���'0'��' '��ʾ Ʃ��09.10��9.10
               {
                 Write_Cmd(0x8c);
                 Write_Data(' ');
                 if(danjia_array[1]=='0')
                  Write_Data(' ');                
               }

           }
           
          }
       }//<----(27����)
       if(key_val==16)            //����ȥƤ��������ȥƤ״̬   ��־M
      {                           //��⵽�ڶ��ΰ��£���ȥƤ״̬��ʧ
        if(qupi_flag==0)          //���϶������ٰ������
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
      }//<----��16������
      if(key_val==10)//�ۼӼ���־��A
      {
       
          leijia_flag=1;
          leijia_count++;
          Write_Cmd(0x82);
          Write_Data(':');
          Write_Data('A');    //<----�ۼ�ģʽӦֻ����������˳�
          if(leijia_count<10)
          {
          Write_Data(leijia_count+0x30);
          }
          else if(leijia_count>=10&&leijia_count<=99)
          {
          Write_Data(leijia_count/10+0x30);
          Write_Data(leijia_count%10+0x30);//<---�����ۼƴ��������999��
          }
          else
          {
          Write_Data(leijia_count/100+0x30);
          Write_Data(leijia_count/10%10+0x30);//<---�����ۼƴ��������999��,1000�κ���ʾ�쳣
          Write_Data(leijia_count%10+0x30);//<---�����ۼƴ��������999��  
          }
          multiply(danjia_array_yunsuan,Weight_Shiwu_array,recent_price);
            for(i=0;i<11;i++)
           {
             if(recent_price[i]=='\0')
             {
               break;
             }
           }//<-----------��ʱi��ʾ���м�λ���֣�����i�Ĵ�С�����ж�
            price_num=i;           
            price_value(price_num);
      }//<----(10�ۼӼ�)
      if(key_val==22)//�����
      {
       leijia_flag=0;
       price_end=0;
       Write_Cmd(0x82);
       Write_Data(':');
       Write_Data(' ');    //<----�ۼ�ģʽӦֻ����������˳�
       Write_Data(' ');
       Write_Data(' ');
       Write_Data(' ');    //<----�ۼӴ�������       
      }//<----(22�����)
    
    }//<---�ж��Ƿ��а�������
    
     //��������������㷨����λ����ߣ�ò��msp430�г˷������ɲ��ᣬ�´�ѧ��
     
     Write_Cmd(0x9a);
     Write_Data(':');
     for(i=0;i<5;i++)
     {Write_Data(' ');} 
     Write_Data('0');
     Write_Data('.');
     Write_Data('0');
     Write_Data('0');                    //����ı�۸�ǰ���Ƚ�֮ǰ����ˢ��
     multiply(danjia_array_yunsuan,Weight_Shiwu_array,recent_price);
      for(i=0;i<11;i++)
     {
       if(recent_price[i]=='\0')
       {
         break;
       }
     }//<-----------��ʱi��ʾ���м�λ���֣�����i�Ĵ�С�����ж�
      price_num=i;
    show_price(price_num);
     
 }//<---while��ѭ������
}
void price_value(unsigned char i)//<--�����ǰֵ�����Ҹ���price_end
{  
        if(i>8)
       {
        price_end=10000;
       }
       else if(i==8)                                      //<-------------�޴�����ֻ��ö��
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
void show_price(unsigned char i)            //��Դ����˷����õ�λ������ʾ���ϵõ���Ӧ��ʾ
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
       else if(i==8)                                      //<-------------�޴�����ֻ��ö��
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
       else if(i==7)                                      //<-------------�޴�����ֻ��ö��
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
   {//�������ۼ�״̬�µ��������    
         if(price_end>=1000000)     //����ۼƽ�����9999.99���쳣��ʾ
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
void multiply(unsigned char* a,unsigned char* b,unsigned char* c)//���۵�����5λ*����4λ=10λ��c����11λ
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
         Write_Data(HX711_Buffer%100/10 + 0x30);   //----->���Դ��룬���ڵ�һ�β��ɾ�˺���������
         Write_Data(HX711_Buffer%10 + 0x30);       //----->����д�����Ծ��Բ�ɾ
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
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi;	//��ȡʵ���AD������ֵ��
	
	        Weight_Shiwu = (unsigned int)((float)Weight_Shiwu/2.10+0.05);           
             //����ʵ���ʵ������
             //��Ϊ��ͬ�Ĵ������������߲�һ������ˣ�ÿһ����������Ҫ���������2.15���������
             //�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
             //������Գ���������ƫСʱ����С����ֵ��
             //����ֵһ����2.15��������֮�䡣�򴫸�����ͬ������
             //+0.05��Ϊ����������ٷ�λ
		BEEPCMD;
		if(  Weight_Shiwu > 20000 )
		{
			BEEP_L;				//�򿪾���	
		}
		else
		{
			BEEP_H;			        //�رվ���
		}
	}
	else if(HX711_Buffer < Weight_Maopi - 30) 
	{
		   	BEEP_L;		               //����������
	}

}
//****************************************************
//��ȡëƤ����
//****************************************************
  void Get_Maopi()
{
	HX711_Buffer = HX711_Read();
	Weight_Maopi = HX711_Buffer/100;	 			
}

/*///////////////////////������//////////////////////////////
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

uchar return_tree()//������״̬
{
  int j,i;
  for(j=0;j<6;j++)              //���ĵ�һλ��ԶΪ1
   {
     if(tree[j]=='0')
     {
       if(tree[j-1]=='5')       //ȥƤ
       {
         for(i=0;i<=j-2;i++)
         {
           if(tree[i]=='6')
              return '7';
         }
         return tree[j-1];
       }
       else if(tree[j-1]=='6')        //�ۼ�
       {
         for(i=0;i<=j-2;i++)        
         {
           if(tree[i]=='5') 
              return '7';              //����7�����ۼ���ȥƤ����ͬʱ����
         }                           //�п����Ż������Կ��Ż�������
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


/////////////////////////////////����ɨ��״̬�޸������޸ĳ���///////////////////////////////////////
 void change_state()
{
   Key_Event();
   uchar value;                                   //value����0~9��ֵ����Ϊ10ʱΪС����
   uchar state;
   if(key_Flag == 1)
   {
     key_Flag=0;
     if(key_val<=3||7<=key_val<=9|| 13<=key_val<=15||19<=key_val<=20)//Ϊ���ּ�
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
       {value=10;}                                      //����С����
       
       state=return_tree();
       if(state=='2')
       shuru_cunchu_change(value);                            
       if(state=='3')
       shuru_cunchu_change(value);    
     }
      if(key_val==4)
        qingchu();
      if(key_val==27)//�����
      {
        if(return_tree()=='2')
        {
         copy_danjia_array();  
        }
        set_tree('2');
      }
      if(key_val==21)//�洢��
        set_tree('3');
      if(key_val==28)//��Ǯ��
        set_tree('4');
      if(key_val==16)//ȥƤ��
        set_tree('5');
      if(key_val==10)//�ۼƼ�
        set_tree('6');
      if(key_val==10)//�����
        clear_tree();
      M_jian();

        
   }
     
}


//////////////////////////////////////////��״̬��ʾ����//////////////////////////////////////////
   void copy_danjia_array()            //��danjia_array_xiugai[6]�е�ֵ������danjia_array[6]��
   {                                   //��danjia_array��¼��ˢ����ʾ
     int i;
     for(i=0;i<6;i++)
     {
      danjia_array[i]=danjia_array_xiugai[i]; 
     }
    show(danjia_array);     
   }
  void shuru_cunchu_change(uchar value)                //��״̬�����ϸ���XXX.XX��ʽ����
  {
    int i;
    if(adress<=6)                                     //������ַ����6����Ԫ����������Ч
    {
        if(adress==4&&adress_flag==0)                 //�״����룬danjia_array_xiugai[5]����
        {
          for(i=0;i<6;i++)
          {
            danjia_array_xiugai[i]='0';               
          }
          danjia_array_xiugai[i]='.';
          show(danjia_array_xiugai);          
        }
        if(adress==5&&adress_flag==1)                //����С���㴦���⴦��
        {
          adress++;adress_flag=0
        }
        else
        {
          Write_Cmd(0x88+adress);                       //�տ�ʼ�����������adress��Ϊ��ʼֵ4
          if(adress_flag==0)                            //adress_flag��Ϊ��ʼֵ0
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
  void qingchu()                                   //�ú�������Ԥ��
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
 void show(uchar *s)          //��������ʾ������ʾ�ĵ��۽��
 {
   Write_Cmd(0x8c);
   for(i=0;i<6;i++)
   {
     Write_Data(*(s+i));
   }
 }

////////////////////////////AT24C16ֵ����////////////////////////////////////////////////////

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
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi;	//��ȡʵ���AD������ֵ��
	
		Weight_Shiwu = (unsigned int)((float)Weight_Shiwu/1.94+0.05);           
             //����ʵ���ʵ������
             //��Ϊ��ͬ�Ĵ������������߲�һ������ˣ�ÿһ����������Ҫ���������2.15���������
             //�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
             //������Գ���������ƫСʱ����С����ֵ��
             //����ֵһ����2.15��������֮�䡣�򴫸�����ͬ������
             //+0.05��Ϊ����������ٷ�λ
		BEEPCMD;
		if(  Weight_Shiwu > 20000 )
		{
			BEEP_L;				//�򿪾���	
		}
		else
		{
			BEEP_H;			        //�رվ���
		}
	}
	else if(HX711_Buffer < Weight_Maopi - 30)
	{
		   	BEEP_L;		               //����������
	}

}
//****************************************************
//��ȡëƤ����
//****************************************************
  void Get_Maopi()
{
	HX711_Buffer = HX711_Read();
	Weight_Maopi = HX711_Buffer/100;	 			
}

*/

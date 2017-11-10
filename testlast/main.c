#include "inc/hw_types.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/pushbutton.h"
#include "drivers/kitronix320x240x16_ssd2119_8bit.h"
#include "drivers/touch.h"
#include "drivers/set_pinout.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//*********************************************************
// �����������ж���� tDisplay ��ʾ�豸����
//*********************************************************
extern const tDisplay g_sKitronix320x240x16_SSD2119;
//*********************************************************
// ��ǰ����Ҫʹ�õ��Ŀؼ���������Ϊ�����ؼ������ܻ��໥���ã�
//*********************************************************
extern tPushButtonWidget goleft;
extern tPushButtonWidget goright;
extern tCanvasWidget z1;//��߻����ؼ�1
extern tCanvasWidget z2;//��߻����ؼ�2
extern tCanvasWidget z3;//��߻����ؼ�3
extern tCanvasWidget y1;//�ұ߻����ؼ�1
extern tCanvasWidget y2;//�ұ߻����ؼ�2
extern tCanvasWidget y3;//�ұ߻����ؼ�3
extern tCanvasWidget c1;
extern tCanvasWidget c2;
extern tCanvasWidget score;
extern tCanvasWidget fenshu;
extern char g_pucImage1[];
extern char g_pucImage2[];
//*********************************************************
// ��ǰ������ť����ʱ�Ĵ���������
//*********************************************************
void OnButtonPress1(tWidget *pWidget);
void OnButtonPress2(tWidget *pWidget);
long seed;
int a,w=3,k=0,flag=1;
volatile unsigned long ulLoop;
unsigned long DelayTime=5000000;
char t[2]={"00"};
//*********************************************************
// ������������ؼ���
//*********************************************************

//����ť
RectangularButton(goleft,WIDGET_ROOT, 0, 0,
&g_sKitronix320x240x16_SSD2119, 10, 190, 80, 45,
(PB_STYLE_OUTLINE  |
PB_STYLE_TEXT | PB_STYLE_FILL ),
ClrDarkBlue, ClrBlue, ClrWhite, ClrWhite,
&g_sFontCmss22b, "Left", 0, 0, 0, 0,
OnButtonPress1);
//���Ұ�ť
RectangularButton(goright,WIDGET_ROOT, 0, 0,
&g_sKitronix320x240x16_SSD2119, 125, 190, 80, 45,
(PB_STYLE_OUTLINE |
PB_STYLE_TEXT | PB_STYLE_FILL ),
ClrDarkBlue, ClrBlue, ClrWhite, ClrWhite,
&g_sFontCmss22b, "Right", 0, 0, 0, 0,
OnButtonPress2);

Canvas(z1, /*�ؼ���*/ WIDGET_ROOT, /* ���ؼ�*/ 0, /* ��һ���ؼ�*/ 0, /*�ӿؼ�*/
&g_sKitronix320x240x16_SSD2119, /* ��ʾ�豸����*/
20, /* �ؼ� X λ��*/ 10, /* �ؼ� Y λ��*/ 60, /* �ؼ����*/ 40, /* �ؼ��߶�*/
CANVAS_STYLE_FILL,// �ؼ����
0, /* �����ɫ*/ 0, /* �߿���ɫ*/ 0, /* ������ɫ*/ 0, /*��������*/ 0, /*��ʾ������*/
0, /* ��ʾ��ͼƬ*/ 0); /* WIDGET_MSG_PAINT �¼�������*/

Canvas(z2, /* �ؼ���*/ WIDGET_ROOT, /* ���ؼ�*/ 0, /* ��һ���ؼ�*/ 0, /* �ӿؼ�*/
&g_sKitronix320x240x16_SSD2119, /* ��ʾ�豸����*/
20, /* �ؼ� X λ��*/ 60, /* �ؼ� Y λ��*/ 60, /* �ؼ����*/ 40, /* �ؼ��߶�*/
CANVAS_STYLE_FILL,/* �ؼ����*/
0, /* �����ɫ*/ 0, /* �߿���ɫ*/ 0, /* ������ɫ*/ 0, /*��������*/ 0, /* ��ʾ������*/
0, /* ��ʾ��ͼƬ*/ 0); /* WIDGET_MSG_PAINT �¼�������*/

Canvas(z3, /* �ؼ���*/ WIDGET_ROOT, /* ���ؼ�*/ 0, /* ��һ���ؼ�*/ 0, /* �ӿؼ�*/
&g_sKitronix320x240x16_SSD2119, // ��ʾ�豸����
20, /* �ؼ� X λ��*/ 110, /* �ؼ� Y λ��*/ 60, /* �ؼ����*/ 40, /*�ؼ��߶�*/
CANVAS_STYLE_FILL,// �ؼ����
0, /* �����ɫ*/ 0, /* �߿���ɫ*/ 0, /* ������ɫ*/ 0, /*��������*/ 0, /* ��ʾ������*/
0, /* ��ʾ��ͼƬ*/ 0); /* WIDGET_MSG_PAINT �¼�������*/

Canvas(y1, /* �ؼ���*/ WIDGET_ROOT, /* ���ؼ�*/ 0, /* ��һ���ؼ�*/ 0, /*�ӿؼ�*/
&g_sKitronix320x240x16_SSD2119, /* ��ʾ�豸����*/
120, /* �ؼ� X λ��*/ 10, /* �ؼ� Y λ��*/ 60, /* �ؼ����*/ 40, /* �ؼ��߶�*/
CANVAS_STYLE_FILL,/* �ؼ����*/
0, /* �����ɫ*/ 0, /* �߿���ɫ*/ 0, /* ������ɫ*/ 0, /*��������*/ 0, /* ��ʾ������*/
0, /* ��ʾ��ͼƬ*/ 0); /* WIDGET_MSG_PAINT �¼�������*/

Canvas(y2, /* �ؼ���*/ WIDGET_ROOT, /* ���ؼ�*/ 0, /* ��һ���ؼ�*/ 0, /* �ӿؼ�*/
&g_sKitronix320x240x16_SSD2119, /* ��ʾ�豸����*/
120, /* �ؼ� X λ��*/ 60, /* �ؼ� Y λ��*/ 60, /*�ؼ����*/ 40, /* �ؼ��߶�*/
CANVAS_STYLE_FILL,/* �ؼ����*/
0, /* �����ɫ*/ 0, /* �߿���ɫ*/ 0, /* ������ɫ*/ 0, /*��������*/ 0, /* ��ʾ������*/
0, /* ��ʾ��ͼƬ*/ 0); /* WIDGET_MSG_PAINT �¼�������*/

Canvas(y3, /* �ؼ���*/ WIDGET_ROOT, /* ���ؼ�*/ 0, /* ��һ���ؼ�*/ 0, /* �ӿؼ�*/
&g_sKitronix320x240x16_SSD2119, /* ��ʾ�豸����*/
120, /* �ؼ� X λ��*/ 110, /* �ؼ� Y λ��*/ 60, /*�ؼ����*/ 40, /* �ؼ��߶�*/
CANVAS_STYLE_FILL,/* �ؼ����*/
0, /* �����ɫ*/ 0, /* �߿���ɫ*/ 0, /* ������ɫ*/ 0, /*��������*/ 0, /* ��ʾ������*/
0, /* ��ʾ��ͼƬ*/ 0); /* WIDGET_MSG_PAINT �¼�������*/

Canvas(c1, /* �ؼ���*/ WIDGET_ROOT, /* ���ؼ�*/ 0, /* ��һ���ؼ�*/ 0, /* �ӿؼ�*/
&g_sKitronix320x240x16_SSD2119, /* ��ʾ�豸����*/
20, /* �ؼ� X λ��*/ 150, /* �ؼ� Y λ��*/ 60, /*�ؼ����*/ 40, /* �ؼ��߶�*/
CANVAS_STYLE_FILL|CANVAS_STYLE_IMG,/* �ؼ����*/
ClrRed, /* �����ɫ*/ 0, /* �߿���ɫ*/ 0, /* ������ɫ*/ 0, /*��������*/ 0, /* ��ʾ������*/
g_pucImage2 , /* ��ʾ��ͼƬ*/ OnButtonPress1); /* WIDGET_MSG_PAINT �¼�������*/

Canvas(c2, /* �ؼ���*/ WIDGET_ROOT, /* ���ؼ�*/ 0, /* ��һ���ؼ�*/ 0, /* �ӿؼ�*/
&g_sKitronix320x240x16_SSD2119, /* ��ʾ�豸����*/
120, /* �ؼ� X λ��*/ 150, /* �ؼ� Y λ��*/ 60, /*�ؼ����*/ 40, /* �ؼ��߶�*/
CANVAS_STYLE_FILL,/* �ؼ����*/
0, /* �����ɫ*/ 0, /* �߿���ɫ*/ 0, /* ������ɫ*/ 0, /*��������*/ 0, /* ��ʾ������*/
0, /* ��ʾ��ͼƬ*/ OnButtonPress2); /* WIDGET_MSG_PAINT �¼�������*/

Canvas(score, /* �ؼ���*/ WIDGET_ROOT, /* ���ؼ�*/ 0, /* ��һ���ؼ�*/ 0, /* �ӿؼ�*/
&g_sKitronix320x240x16_SSD2119, /* ��ʾ�豸����*/
220, /* �ؼ� X λ��*/ 80, /* �ؼ� Y λ��*/100, /*�ؼ����*/ 60, /* �ؼ��߶�*/
CANVAS_STYLE_FILL|CANVAS_STYLE_TEXT,/* �ؼ����*/
ClrDarkOrange, /* �����ɫ*/ 0, /* �߿���ɫ*/ ClrWhite, /* ������ɫ*/ &g_sFontCmss22b, /*��������*/ t, /* ��ʾ������*/
0, /* ��ʾ��ͼƬ*/ 0); /* WIDGET_MSG_PAINT �¼�������*/

Canvas(fenshu, /* �ؼ���*/ WIDGET_ROOT, /* ���ؼ�*/ 0, /* ��һ���ؼ�*/ 0, /* �ӿؼ�*/
&g_sKitronix320x240x16_SSD2119, /* ��ʾ�豸����*/
220, /* �ؼ� X λ��*/ 20, /* �ؼ� Y λ��*/ 100, /*�ؼ����*/ 60, /* �ؼ��߶�*/
CANVAS_STYLE_FILL|CANVAS_STYLE_TEXT,/* �ؼ����*/
ClrDarkOrange, /* �����ɫ*/ 0, /* �߿���ɫ*/ ClrWhite, /* ������ɫ*/ &g_sFontCmss22b, /*��������*/"Score:", /* ��ʾ������*/
0, /* ��ʾ��ͼƬ*/ 0); /* WIDGET_MSG_PAINT �¼�������*/

void OnButtonPress1(tWidget *pWidget)
{   
 
	
	CanvasFillColorSet(&c1, ClrRed);
	CanvasImageSet(&c1,g_pucImage2);
			 CanvasImageOn(&c1);
	WidgetAdd(WIDGET_ROOT,(tWidget *)&c1);	
	WidgetPaint((tWidget *)&c1);
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&c2, 0);
	 CanvasImageOff(&c2);
	WidgetAdd(WIDGET_ROOT,(tWidget *)&c2);	
	WidgetPaint((tWidget *)&c2);
	WidgetMessageQueueProcess();	
	w=3;
}

void OnButtonPress2(tWidget *pWidget)
{

	
	CanvasFillColorSet(&c1, 0);
			 CanvasImageOff(&c1);
	WidgetAdd(WIDGET_ROOT,(tWidget *)&c1);	
	WidgetPaint((tWidget *)&c1);	
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&c2, ClrRed);
          CanvasImageSet(&c2,g_pucImage2);
			 CanvasImageOn(&c2);
	WidgetAdd(WIDGET_ROOT,(tWidget *)&c2);	
	WidgetPaint( (tWidget *)&c2);
	WidgetMessageQueueProcess();
	w=2;

}
void Delay(void)
{
		int u;
		for(u=0;u<DelayTime;u++)
		{}
}
void SpeedUP(void)
{
		
		if(DelayTime>=5000000)
		{
				DelayTime=DelayTime-500000;
		}
		else if(DelayTime>=2000000)
		{
				DelayTime=DelayTime-300000;
		}
		else if(DelayTime>=100000)
		{
				DelayTime=DelayTime-10000;
		}
		else if(DelayTime>=10000)
		{
				DelayTime=DelayTime-1000;
		}
		else if(DelayTime>=1000)
		{
				DelayTime=DelayTime-500;
		}
		else if(DelayTime>=100)
		{
				DelayTime=DelayTime-10;
		}
		else if(DelayTime>=50)
		{
				DelayTime=DelayTime-5;
		}
		
}


void RUN()
{
	  WidgetAdd(WIDGET_ROOT,(tWidget *)&score);	
	         WidgetPaint((tWidget *)&score);
			     
if(flag == 1){
  
		seed = SysTickValueGet( );
	  srand(seed);  
     a=rand()%10; 		

	if(a>5)
	{    
		CanvasFillColorSet(&z1, 0);
                CanvasImageOff(&z1);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&z1);	
	WidgetPaint((tWidget *)&z1);
	WidgetMessageQueueProcess();		
		CanvasFillColorSet(&z2, 0);
                CanvasImageOff(&z2);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&z2);	
	WidgetPaint((tWidget *)&z2);
	WidgetMessageQueueProcess();
		CanvasFillColorSet(&z3, 0);
                CanvasImageOff(&z3);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&z3);	
	WidgetPaint((tWidget *)&z3);
	WidgetMessageQueueProcess();
		CanvasFillColorSet(&y2, 0);
                CanvasImageOff(&y2);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y2);	
	WidgetPaint((tWidget *)&y2);
	WidgetMessageQueueProcess();
		CanvasFillColorSet(&y3, 0);
                CanvasImageOff(&y3);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y3);	
	WidgetPaint((tWidget *)&y3);
	WidgetMessageQueueProcess();
	  CanvasFillColorSet(&y1, ClrRed);
          CanvasImageSet(&y1,g_pucImage1);
                CanvasImageOn(&y1);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y1);	
	WidgetPaint((tWidget *)&y1);
	WidgetMessageQueueProcess();
		
  Delay();
	Delay();
			CanvasFillColorSet(&y1, 0);
                CanvasImageOff(&y1);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y1);	
	WidgetPaint((tWidget *)&y1);
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&y3, 0);
                CanvasImageOff(&y3);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y3);	
	WidgetPaint((tWidget *)&y3);
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&z1, 0);
                CanvasImageOff(&z1);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&z1);	
	WidgetPaint((tWidget *)&z1);
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&z2, 0);
                CanvasImageOff(&z2);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&z2);	
	WidgetPaint((tWidget *)&z2);
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&z3, 0);
                CanvasImageOff(&z3);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&z3);	
	WidgetPaint((tWidget *)&z3);
	WidgetMessageQueueProcess();
	  CanvasFillColorSet(&y2, ClrRed);
          CanvasImageSet(&y2,g_pucImage1);
                CanvasImageOn(&y2);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y2);	
	WidgetPaint((tWidget *)&y2);
	WidgetMessageQueueProcess();
	  Delay();	
	  Delay();
		CanvasFillColorSet(&y1, 0);
                CanvasImageOff(&y1);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y1);	
	WidgetPaint((tWidget *)&y1);
	WidgetMessageQueueProcess();
			CanvasFillColorSet(&y2, 0);
                CanvasImageOff(&y2);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y2);	
	WidgetPaint((tWidget *)&y2);
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&z1, 0);
                CanvasImageOff(&z1);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&z1);	
	WidgetPaint((tWidget *)&z1);
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&z2, 0);
                CanvasImageOff(&z2);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&z2);	
	WidgetPaint((tWidget *)&z2);
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&z3, 0);
                CanvasImageOff(&z3);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&z3);	
	WidgetPaint((tWidget *)&z3);
	WidgetMessageQueueProcess();
	  CanvasFillColorSet(&y3, ClrRed);
          CanvasImageSet(&y3,g_pucImage1);
                CanvasImageOn(&y3);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y3);	
	WidgetPaint((tWidget *)&y3);
	WidgetMessageQueueProcess();
       Delay();
			k=2;
		CanvasFillColorSet(&y3,0);
                CanvasImageOff(&y3);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y3);	
	WidgetPaint((tWidget *)&y3);
	WidgetMessageQueueProcess();
		}
		else
		{   
			CanvasFillColorSet(&y1, 0);
                CanvasImageOff(&y1);
				WidgetAdd(WIDGET_ROOT,(tWidget *)&y1);	
	WidgetPaint((tWidget *)&y1); 
	WidgetMessageQueueProcess();	
			CanvasFillColorSet(&y2, 0);
                CanvasImageOff(&y2);
				WidgetAdd(WIDGET_ROOT,(tWidget *)&y2);	
	WidgetPaint((tWidget *)&y2);
	WidgetMessageQueueProcess();	
			CanvasFillColorSet(&y3, 0);
                CanvasImageOff(&y3);
				WidgetAdd(WIDGET_ROOT,(tWidget *)&y3);	
	WidgetPaint((tWidget *)&y3);
	WidgetMessageQueueProcess();
			CanvasFillColorSet(&z2, 0);
                CanvasImageOff(&z2);
				WidgetAdd(WIDGET_ROOT,(tWidget *)&z2);	
	WidgetPaint((tWidget *)&z2);
	WidgetMessageQueueProcess();	
			CanvasFillColorSet(&z3, 0);
                CanvasImageOff(&z3);
				WidgetAdd(WIDGET_ROOT,(tWidget *)&z3);	
	WidgetPaint((tWidget *)&z3);
	WidgetMessageQueueProcess();	
			CanvasFillColorSet(&z1,ClrRed);
                        CanvasImageSet(&z1,g_pucImage1);
                        CanvasImageOn(&z1);
			WidgetAdd(WIDGET_ROOT,(tWidget *)&z1);	
	WidgetPaint((tWidget *)&z1);
	WidgetMessageQueueProcess();	
	      Delay();
				Delay();
				CanvasFillColorSet(&z1, 0);
                        CanvasImageOff(&z1);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&z1);	
	WidgetPaint((tWidget *)&z1);
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&z3, 0);
                        CanvasImageOff(&z3);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&z3);	
	WidgetPaint((tWidget *)&z1);
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&y1, 0);
                        CanvasImageOff(&y1);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y1);	
	WidgetPaint((tWidget *)&y1);
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&y2, 0);
                        CanvasImageOff(&y2);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y2);	
	WidgetPaint((tWidget *)&y2);
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&y3, 0);
                        CanvasImageOff(&y3);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y3);	
	WidgetPaint((tWidget *)&y3);
	WidgetMessageQueueProcess();
	   CanvasFillColorSet(&z2,ClrRed);
           CanvasImageSet(&z2,g_pucImage1);
           CanvasImageOn(&z2);
		 WidgetAdd(WIDGET_ROOT,(tWidget *)&z2);	
	WidgetPaint((tWidget *)&z2);
	WidgetMessageQueueProcess();
	Delay();		
	Delay();
	CanvasFillColorSet(&z1, 0);
        CanvasImageOff(&z1);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&z1);	
	WidgetPaint((tWidget *)&z1);
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&y1, 0);
        CanvasImageOff(&y1);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y1);	
	WidgetPaint((tWidget *)&y1);
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&y2, 0);
        CanvasImageOff(&y2);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y2);	
	WidgetPaint((tWidget *)&y2);
	WidgetMessageQueueProcess();
	CanvasFillColorSet(&y3, 0);
        CanvasImageOff(&y3);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&y3);	
	WidgetPaint((tWidget *)&y3);
	WidgetMessageQueueProcess();
	  CanvasFillColorSet(&z2,0);
        CanvasImageOff(&z2);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&z2);	
	WidgetPaint((tWidget *)&z2);
	WidgetMessageQueueProcess();
		CanvasFillColorSet(&z3,ClrRed);
                CanvasImageSet(&z3,g_pucImage1);
                CanvasImageOn(&z3);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&z3);	
	WidgetPaint((tWidget *)&z3);
	WidgetMessageQueueProcess();
		
	Delay();
	k=3;
	CanvasFillColorSet(&z3,0);
        CanvasImageOff(&z3);
		WidgetAdd(WIDGET_ROOT,(tWidget *)&z3);	
	WidgetPaint((tWidget *)&z3);
	WidgetMessageQueueProcess();
			}
			
			         t[1]++;
         			if(t[1]=='9'){
                   t[0]++;
                   t[1]='0';								
								}
								
	      Delay();	
		  	Delay();	
			  Delay(); 
	 
	}
	}

//*********************************************************
// ������
//*********************************************************
int main(void)
{
// ����ϵͳ������ 50MHz��
SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL |
SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
// Ϊ 9B96 ���������ܽš� ����ҪΪʹ�ܸ� GPIO��
PinoutSet();
// ȫ�������жϡ�
IntMasterEnable();
// ��ʼ����ʾ��������
Kitronix320x240x16_SSD2119Init();
// ��ʼ��������������
TouchScreenInit();
// ���ô������������Ļص�����Ϊͼ�ο�Ĵ���������
TouchScreenCallbackSet(WidgetPointerMessage);
// �� g_sBackground ���µĿؼ�����ؼ�����
  WidgetAdd(WIDGET_ROOT, (tWidget *)&goleft);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&goright);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&c1);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&c2);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&z1);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&z2);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&z3);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&y1);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&y2);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&y3);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&score);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&fenshu);
	
	
// ���ƿؼ����е����пؼ���
WidgetPaint(WIDGET_ROOT);
  SysTickPeriodSet(6000000UL);
	SysTickEnable( );
// ��ѭ����
while(1)
{
// �������пؼ��¼���


	if((w==3&&k==3)||(w==2&&k==2))
	
          {	
						 
						break;}
	   else 
		 {
			 	WidgetMessageQueueProcess();
			 
			 RUN();
		 }
		 
	
}

}


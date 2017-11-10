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
// 声明驱动库中定义的 tDisplay 显示设备对象。
//*********************************************************
extern const tDisplay g_sKitronix320x240x16_SSD2119;
//*********************************************************
// 提前声明要使用到的控件。（后文为建立控件树可能会相互引用）
//*********************************************************
extern tPushButtonWidget goleft;
extern tPushButtonWidget goright;
extern tCanvasWidget z1;//左边画布控件1
extern tCanvasWidget z2;//左边画布控件2
extern tCanvasWidget z3;//左边画布控件3
extern tCanvasWidget y1;//右边画布控件1
extern tCanvasWidget y2;//右边画布控件2
extern tCanvasWidget y3;//右边画布控件3
extern tCanvasWidget c1;
extern tCanvasWidget c2;
extern tCanvasWidget score;
extern tCanvasWidget fenshu;
extern char g_pucImage1[];
extern char g_pucImage2[];
//*********************************************************
// 提前声明按钮按下时的处理凼数。
//*********************************************************
void OnButtonPress1(tWidget *pWidget);
void OnButtonPress2(tWidget *pWidget);
long seed;
int a,w=3,k=0,flag=1;
volatile unsigned long ulLoop;
unsigned long DelayTime=5000000;
char t[2]={"00"};
//*********************************************************
// 创建并定义各控件。
//*********************************************************

//向左按钮
RectangularButton(goleft,WIDGET_ROOT, 0, 0,
&g_sKitronix320x240x16_SSD2119, 10, 190, 80, 45,
(PB_STYLE_OUTLINE  |
PB_STYLE_TEXT | PB_STYLE_FILL ),
ClrDarkBlue, ClrBlue, ClrWhite, ClrWhite,
&g_sFontCmss22b, "Left", 0, 0, 0, 0,
OnButtonPress1);
//向右按钮
RectangularButton(goright,WIDGET_ROOT, 0, 0,
&g_sKitronix320x240x16_SSD2119, 125, 190, 80, 45,
(PB_STYLE_OUTLINE |
PB_STYLE_TEXT | PB_STYLE_FILL ),
ClrDarkBlue, ClrBlue, ClrWhite, ClrWhite,
&g_sFontCmss22b, "Right", 0, 0, 0, 0,
OnButtonPress2);

Canvas(z1, /*控件名*/ WIDGET_ROOT, /* 父控件*/ 0, /* 下一个控件*/ 0, /*子控件*/
&g_sKitronix320x240x16_SSD2119, /* 显示设备对象*/
20, /* 控件 X 位置*/ 10, /* 控件 Y 位置*/ 60, /* 控件宽度*/ 40, /* 控件高度*/
CANVAS_STYLE_FILL,// 控件风格
0, /* 填充颜色*/ 0, /* 边框颜色*/ 0, /* 文字颜色*/ 0, /*文字字体*/ 0, /*显示的文字*/
0, /* 显示的图片*/ 0); /* WIDGET_MSG_PAINT 事件处理函数*/

Canvas(z2, /* 控件名*/ WIDGET_ROOT, /* 父控件*/ 0, /* 下一个控件*/ 0, /* 子控件*/
&g_sKitronix320x240x16_SSD2119, /* 显示设备对象*/
20, /* 控件 X 位置*/ 60, /* 控件 Y 位置*/ 60, /* 控件宽度*/ 40, /* 控件高度*/
CANVAS_STYLE_FILL,/* 控件风格*/
0, /* 填充颜色*/ 0, /* 边框颜色*/ 0, /* 文字颜色*/ 0, /*文字字体*/ 0, /* 显示的文字*/
0, /* 显示的图片*/ 0); /* WIDGET_MSG_PAINT 事件处理函数*/

Canvas(z3, /* 控件名*/ WIDGET_ROOT, /* 父控件*/ 0, /* 下一个控件*/ 0, /* 子控件*/
&g_sKitronix320x240x16_SSD2119, // 显示设备对象
20, /* 控件 X 位置*/ 110, /* 控件 Y 位置*/ 60, /* 控件宽度*/ 40, /*控件高度*/
CANVAS_STYLE_FILL,// 控件风格
0, /* 填充颜色*/ 0, /* 边框颜色*/ 0, /* 文字颜色*/ 0, /*文字字体*/ 0, /* 显示的文字*/
0, /* 显示的图片*/ 0); /* WIDGET_MSG_PAINT 事件处理函数*/

Canvas(y1, /* 控件名*/ WIDGET_ROOT, /* 父控件*/ 0, /* 下一个控件*/ 0, /*子控件*/
&g_sKitronix320x240x16_SSD2119, /* 显示设备对象*/
120, /* 控件 X 位置*/ 10, /* 控件 Y 位置*/ 60, /* 控件宽度*/ 40, /* 控件高度*/
CANVAS_STYLE_FILL,/* 控件风格*/
0, /* 填充颜色*/ 0, /* 边框颜色*/ 0, /* 文字颜色*/ 0, /*文字字体*/ 0, /* 显示的文字*/
0, /* 显示的图片*/ 0); /* WIDGET_MSG_PAINT 事件处理函数*/

Canvas(y2, /* 控件名*/ WIDGET_ROOT, /* 父控件*/ 0, /* 下一个控件*/ 0, /* 子控件*/
&g_sKitronix320x240x16_SSD2119, /* 显示设备对象*/
120, /* 控件 X 位置*/ 60, /* 控件 Y 位置*/ 60, /*控件宽度*/ 40, /* 控件高度*/
CANVAS_STYLE_FILL,/* 控件风格*/
0, /* 填充颜色*/ 0, /* 边框颜色*/ 0, /* 文字颜色*/ 0, /*文字字体*/ 0, /* 显示的文字*/
0, /* 显示的图片*/ 0); /* WIDGET_MSG_PAINT 事件处理函数*/

Canvas(y3, /* 控件名*/ WIDGET_ROOT, /* 父控件*/ 0, /* 下一个控件*/ 0, /* 子控件*/
&g_sKitronix320x240x16_SSD2119, /* 显示设备对象*/
120, /* 控件 X 位置*/ 110, /* 控件 Y 位置*/ 60, /*控件宽度*/ 40, /* 控件高度*/
CANVAS_STYLE_FILL,/* 控件风格*/
0, /* 填充颜色*/ 0, /* 边框颜色*/ 0, /* 文字颜色*/ 0, /*文字字体*/ 0, /* 显示的文字*/
0, /* 显示的图片*/ 0); /* WIDGET_MSG_PAINT 事件处理函数*/

Canvas(c1, /* 控件名*/ WIDGET_ROOT, /* 父控件*/ 0, /* 下一个控件*/ 0, /* 子控件*/
&g_sKitronix320x240x16_SSD2119, /* 显示设备对象*/
20, /* 控件 X 位置*/ 150, /* 控件 Y 位置*/ 60, /*控件宽度*/ 40, /* 控件高度*/
CANVAS_STYLE_FILL|CANVAS_STYLE_IMG,/* 控件风格*/
ClrRed, /* 填充颜色*/ 0, /* 边框颜色*/ 0, /* 文字颜色*/ 0, /*文字字体*/ 0, /* 显示的文字*/
g_pucImage2 , /* 显示的图片*/ OnButtonPress1); /* WIDGET_MSG_PAINT 事件处理函数*/

Canvas(c2, /* 控件名*/ WIDGET_ROOT, /* 父控件*/ 0, /* 下一个控件*/ 0, /* 子控件*/
&g_sKitronix320x240x16_SSD2119, /* 显示设备对象*/
120, /* 控件 X 位置*/ 150, /* 控件 Y 位置*/ 60, /*控件宽度*/ 40, /* 控件高度*/
CANVAS_STYLE_FILL,/* 控件风格*/
0, /* 填充颜色*/ 0, /* 边框颜色*/ 0, /* 文字颜色*/ 0, /*文字字体*/ 0, /* 显示的文字*/
0, /* 显示的图片*/ OnButtonPress2); /* WIDGET_MSG_PAINT 事件处理函数*/

Canvas(score, /* 控件名*/ WIDGET_ROOT, /* 父控件*/ 0, /* 下一个控件*/ 0, /* 子控件*/
&g_sKitronix320x240x16_SSD2119, /* 显示设备对象*/
220, /* 控件 X 位置*/ 80, /* 控件 Y 位置*/100, /*控件宽度*/ 60, /* 控件高度*/
CANVAS_STYLE_FILL|CANVAS_STYLE_TEXT,/* 控件风格*/
ClrDarkOrange, /* 填充颜色*/ 0, /* 边框颜色*/ ClrWhite, /* 文字颜色*/ &g_sFontCmss22b, /*文字字体*/ t, /* 显示的文字*/
0, /* 显示的图片*/ 0); /* WIDGET_MSG_PAINT 事件处理函数*/

Canvas(fenshu, /* 控件名*/ WIDGET_ROOT, /* 父控件*/ 0, /* 下一个控件*/ 0, /* 子控件*/
&g_sKitronix320x240x16_SSD2119, /* 显示设备对象*/
220, /* 控件 X 位置*/ 20, /* 控件 Y 位置*/ 100, /*控件宽度*/ 60, /* 控件高度*/
CANVAS_STYLE_FILL|CANVAS_STYLE_TEXT,/* 控件风格*/
ClrDarkOrange, /* 填充颜色*/ 0, /* 边框颜色*/ ClrWhite, /* 文字颜色*/ &g_sFontCmss22b, /*文字字体*/"Score:", /* 显示的文字*/
0, /* 显示的图片*/ 0); /* WIDGET_MSG_PAINT 事件处理函数*/

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
// 主凼数
//*********************************************************
int main(void)
{
// 设置系统工作在 50MHz。
SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL |
SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
// 为 9B96 开发板分配管脚。 （主要为使能各 GPIO）
PinoutSet();
// 全局允许中断。
IntMasterEnable();
// 初始化显示驱动程序。
Kitronix320x240x16_SSD2119Init();
// 初始化触摸驱动程序。
TouchScreenInit();
// 设置触摸驱动凼数的回调凼数为图形库的处理凼数。
TouchScreenCallbackSet(WidgetPointerMessage);
// 将 g_sBackground 以下的控件加入控件树。
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
	
	
// 绘制控件树中的所有控件。
WidgetPaint(WIDGET_ROOT);
  SysTickPeriodSet(6000000UL);
	SysTickEnable( );
// 主循环。
while(1)
{
// 处理所有控件事件。


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


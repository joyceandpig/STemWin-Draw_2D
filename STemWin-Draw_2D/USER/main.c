#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "ILI93xx.h"
#include "key.h"
#include "malloc.h" 
#include "usmart.h" 
#include "GUI.h"
#include "touch.h"
#include "includes.h"
#include "math.h"

//ALIENTEK Mini STM32开发板范例代码27
//内存管理实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
extern void _MY_GetTouchPos(void);


/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
#define START_TASK_PRIO      		20        //开始任务的优先级设置为最低
#define START_STK_SIZE  				64        //设置任务堆栈大小
OS_STK START_TASK_STK[START_STK_SIZE];    //任务堆栈	
void start_task(void *pdata);	            //任务函数

#define LED_TASK_PRIO      			9        //开始任务的优先级设置为最低
#define LED_STK_SIZE  					64        //设置任务堆栈大小
OS_STK LED_TASK_STK[LED_STK_SIZE];    //任务堆栈	
void led_task(void *pdata);	            //任务函数

#define EMWIN_DEMO_TASK_PRIO    8        //开始任务的优先级设置为最低
#define EMWIN_STK_SIZE  				3096        //设置任务堆栈大小
OS_STK EMWIN_TASK_STK[EMWIN_STK_SIZE];    //任务堆栈	
void emwin_demo_task(void *pdata);	            //任务函数

#define TOUCH_TASK_PRIO      		10        //开始任务的优先级设置为最低
#define TOUCH_STK_SIZE  				64        //设置任务堆栈大小
OS_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];    //任务堆栈	
void touch_task(void *pdata);	            //任务函数

void BSP_Init(void)
{
	NVIC_Configuration();	 
	delay_init();	    			 //延时函数初始化	  
	uart_init(115200);	 		//串口初始化为9600
	LED_Init();		  				//初始化与LED连接的硬件接口
	TFTLCD_Init();			   	//初始化LCD		 
	tp_dev.init();
//	tp_dev.adjust();
	mem_init();				//初始化内存池

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);
  GUI_Init();
	
}
void main_ui(void)
{
#if 0
	 _MY_GetTouchPos();
#endif
	GUI_SetBkColor(GUI_BLACK);
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringAt("Hello World!", 30, 200);
	GUI_DispStringAt("Hello emWin!", 30, 216);
	GUI_DrawRoundedRect(0,0,200,200,5);
	GUI_DrawRoundedFrame(2,2,180,20,5,2);
}
void disp(void)
{
	GUI_SetColor(GUI_RED);
	GUI_SetBkColor(GUI_BLUE);
	GUI_Clear();
	GUI_SetBkColor(GUI_YELLOW);
	GUI_ClearRect(10,20,30,110); //填充背景色
	GUI_SetBkColor(GUI_BLUE);


	GUI_DrawGradientH(40, 20, 90, 110, GUI_GREEN, GUI_CYAN);//绘制用水平颜色梯度填充的矩形
	GUI_DrawGradientRoundedV(100, 20, 200, 110,5, GUI_YELLOW, GUI_RED);//绘制用垂直颜色梯度填充的圆角矩形
	GUI_DrawRect(5,115,95,205);
	GUI_FillRect(100,115,190,205);//填充矩形，会填充颜色
	GUI_FillRoundedRect(10,210,100,300,10);//绘制圆角矩形,会填充颜色

	GUI_DrawRoundedFrame(140,250,150,280,5,2);//绘制圆角框，指定边框大小,不填充颜色
	GUI_InvertRect(160,250,180,280);//倒转矩形，颜色倒转
	
	GUI_SetPenSize(5);
	GUI_DrawPixel(120,220);//绘制像素点
	GUI_DrawPoint(130,240);//以给定画笔大小绘制一个点
}
void alpha(void)
{
	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();
	GUI_SetColor(GUI_BLACK);
	GUI_DispStringHCenterAt("ALIENEK ALPHA DISPLAY", 120, 1);
	GUI_SetFont(GUI_FONT_16_ASCII);
	
	GUI_EnableAlpha(1);
	GUI_DispStringHCenterAt("aaaaasadadadadaAlphablending that is ok", 120, 61);
	
	GUI_SetAlpha(40);//
	GUI_SetColor(GUI_RED);
	GUI_FillRect(0,20,50,89);
	
	GUI_SetAlpha(80);
	GUI_SetColor(GUI_GREEN);
	GUI_FillRect(60,20,110,89);
	
	GUI_SetAlpha(100);
	GUI_SetColor(GUI_BLUE);
	GUI_FillRect(120,20,170,89);
	GUI_EnableAlpha(0);
}
void line(void)
{
	u8 i,Alpha;
	GUI_DrawLine(10,10,210,10);
	GUI_DrawHLine(20,10,210);
	GUI_DrawVLine(220,20,210);
	
	GUI_SetLineStyle(GUI_LS_DOT);//仅用于画笔大小为1
	GUI_DrawLine(10,30,210,30);
	GUI_SetLineStyle(GUI_LS_SOLID);
	
	
	GUI_SetColor(GUI_BLUE);
	GUI_SetBkColor(GUI_BLACK);
	GUI_FillCircle(100, 50, 49);
	GUI_SetColor(GUI_YELLOW);
	GUI_EnableAlpha(1);
	for (i = 0; i < 100; i++) {
		Alpha = (i * 255 / 100);
		GUI_SetAlpha(Alpha);
		GUI_DrawHLine(i+0, 100 - i, 100 + i);
	}	
	GUI_SetAlpha(0);
	GUI_EnableAlpha(0);
	GUI_SetColor(GUI_MAGENTA);
	GUI_SetFont(&GUI_Font24B_ASCII);
	GUI_SetTextMode(GUI_TM_XOR);
	GUI_DispStringHCenterAt("Alphablending", 100, 30);
}
void polygon(void)
{
	int i,x,y;
	float a ;
	char ac[4];
	I16 aY[240];
	int  Mag, magy = 50, Count = 4;
		int a0, a1;
const unsigned aValues[] = { 100, 135, 190, 240, 340, 360};
const GUI_COLOR aColors[] = { GUI_BLUE, GUI_GREEN, GUI_RED,GUI_CYAN, GUI_MAGENTA, GUI_YELLOW };
	
	const GUI_POINT aPoints[]={
	{40,20},
	{0,20},
	{20,0}
	};
	const GUI_POINT aPoints1[]={
	{0,20},
	{40,20},
	{20,0}
	};
	const GUI_POINT aPointArrow[]={
	{  0, -85},
	{ 10, -85},
	{ 10, -35},
	{ 40, -35},
	{ 0, -5},
	{-40, -35},
	{-10, -35},
	{-10, -85},
	};
	GUI_POINT aPointArrow1[]={
	{ 80, -85},
	{ 50, -45},
	{ 50, -75},
	{  0, -75},
	{  0, -95},
	{ 50, -95},
	{ 50, -125},
	{0,0}
	};
	GUI_POINT aEnlargedPoints[GUI_COUNTOF(aPoints)];    //
	GUI_POINT aMagnifiedPoints[GUI_COUNTOF(aPoints1)];

//2D图形 矩形，渐变色矩形 圆角矩形 画圆  椭圆
	GUI_SetBkColor(GUI_BLUE);    //
	GUI_Clear();
	GUI_SetColor(GUI_YELLOW);
	GUI_SetFont(&GUI_Font24_ASCII);
	GUI_DispStringHCenterAt("ALIENTEK 2D DISPALY",120,10);
	GUI_SetColor(GUI_RED);
	GUI_SetFont(&GUI_Font16_ASCII);
	GUI_SetBkColor(GUI_GREEN);
	GUI_ClearRect(10,50,100,100);  //
	GUI_SetBkColor(GUI_BLUE);    //
	GUI_DrawGradientH(110,50,210,100,0X4117BB,0XC6B6F5);
	GUI_DrawGradientV(10,110,100,150,0X4117BB,0XC6B6F5);
	GUI_DrawGradientRoundedH(120,120,150,150,5,0X4117BB,0XC6B6F5);
	GUI_DrawGradientRoundedV(10,160,120,200,5,0X4117BB,0XC6B6F5); 
	GUI_DrawRect(160,120,200,160);    
	GUI_FillRect(150,170,180,200);    //
	GUI_SetPenSize(5);        //
	GUI_DrawLine(10,160,110,260);      //
	for(i=0;i<50;i+=3) GUI_DrawCircle(170,220,i);    //
	GUI_FillCircle(200,130,10);         
	GUI_SetPenSize(2);
	GUI_SetColor(GUI_CYAN);
	GUI_DrawEllipse(50,240,40,20);
	GUI_SetColor(GUI_MAGENTA);
	GUI_FillEllipse(50,250,30,10);  
	GUI_SetBkColor(GUI_WHITE);

//仪表盘
	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();
	GUI_SetPenSize( 5 );
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_SetFont(&GUI_FontComic18B_ASCII);
	GUI_SetColor(GUI_BLACK);
	GUI_DrawArc( 120,180,100,100,0,180);
	GUI_DrawPoint(120,180);

	for (i=0; i<= 18;i++) {
		a = (i*10)*3.1415926/180;
		x = -100*cos(a)+120;
		y = -141*sin(a)+180;
		if (i%2 == 0){
			GUI_SetPenSize( 5 );
			GUI_DrawPoint(x,y);
			x = -123*cos(a)+120;
			y = -130*sin(a)+180;
			sprintf(ac, "%d", i*10);
			GUI_SetTextAlign(GUI_TA_VCENTER);
			GUI_DispStringHCenterAt(ac,x,y);
		}else{
			GUI_SetPenSize( 4 );
			GUI_DrawPoint(x,y);
		}
	}
	
	GUI_Clear();
	
	//线图
	for(i=0;i<GUI_COUNTOF(aY);i++){
		srand(i);
		aY[i] = rand()%100;
	}
	GUI_ClearRect(0,0,240,320);    //????????????
	GUI_DrawGraph(aY,GUI_COUNTOF(aY),0,150);
	
	GUI_Clear();
	
	//多边形
	GUI_SetTextMode(GUI_TM_TRANS);  
	GUI_SetFont(&GUI_Font16_ASCII); 
	GUI_DispStringHCenterAt("ALIENTEK PLAYGON DISPLAY",120,10);
	GUI_SetColor(GUI_WHITE);
	GUI_SetDrawMode(GUI_DM_XOR);
	GUI_FillPolygon(aPoints,GUI_COUNTOF(aPoints),140,110);//原点位置，终点连接起点
	for(i=1;i<10;i++)
	{
		GUI_EnlargePolygon(aEnlargedPoints,aPoints,GUI_COUNTOF(aPoints),i * 3);//按照指定系数 全方位扩展多边形
		GUI_FillPolygon(aEnlargedPoints,GUI_COUNTOF(aPoints),140,110);     
	}
	GUI_SetDrawMode(GUI_DM_NORMAL); 
	GUI_SetColor(GUI_GREEN);
	for (Mag = 1; Mag <= 4; Mag *= 2, Count /= 2) 
	{
		int magx = 10;
		GUI_MagnifyPolygon(aMagnifiedPoints, aPoints, GUI_COUNTOF(aPoints), Mag); //放大多边形
		for (i = Count; i > 0; i--, magx += 40 * Mag) {
			GUI_FillPolygon(aMagnifiedPoints, GUI_COUNTOF(aPoints), magx, magy);//填充画多边形
		}
		magy += 50 * Mag;
	}
	GUI_SetFont(&GUI_Font8x16);
	GUI_DispStringAt("Polygons of arbitrary shape ", 50, 300);
	GUI_DispStringAt("in any color", 80, 220);
	GUI_SetColor(GUI_DARKRED);
	GUI_FillPolygon (&aPointArrow[0],7,120,320);
	//旋转多边形
	GUI_SetBkColor(GUI_DARKGREEN);
	GUI_SetPenSize(10);
	GUI_SetColor(GUI_DARKGREEN);
	GUI_Clear(); 
	GUI_SetColor(GUI_BLACK);
	
	GUI_RotatePolygon(&aPointArrow1[0],&aPointArrow[0],GUI_COUNTOF(aPointArrow),0);
	GUI_FillPolygon (&aPointArrow1[0],GUI_COUNTOF(aPointArrow1),120,250);
	for(i = 0; i <= 360;i++){
	GUI_SetColor(GUI_DARKGREEN);
	GUI_FillPolygon (&aPointArrow1[0],GUI_COUNTOF(aPointArrow),120,250);//以底色画图，清除原有图案,擦除功能
	GUI_SetColor(GUI_BLACK);
	GUI_RotatePolygon(&aPointArrow1[0],&aPointArrow[0],GUI_COUNTOF(aPointArrow),6*i*3.1415926/180);//把旋转角度后的多边形数据计算出来并保存至目标处，然后调用显示
																														//函数需要弧度，需将角度转化为弧度
	GUI_FillPolygon (&aPointArrow1[0],GUI_COUNTOF(aPointArrow),120,250);
	delay_ms(20);
	}
	//绘制圆形扇区
	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();
	for (i = 0; i < GUI_COUNTOF(aValues); i++) {
		a0 = (i == 0) ?0 :aValues[i - 1];
		a1 = aValues[i];
		GUI_SetColor(aColors[i]);
		GUI_DrawPie(100, 100, 50, a0, a1, 0);//指定圆心，半径，起始角度，终止角度，默认0
		delay_ms(200);
	}
}

int main(void)
{
	BSP_Init();
//	main_ui();
//	disp();
//	alpha();
//	line();
	polygon();//多边形

	OSInit();
	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);//创建起始任务
	OSStart();
}
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr = 0;

//	GUI_Delay(1000);
	OS_ENTER_CRITICAL();
	OSTaskCreate(emwin_demo_task,(void *)0,&EMWIN_TASK_STK[EMWIN_STK_SIZE-1],EMWIN_DEMO_TASK_PRIO);
	OSTaskCreate(touch_task,(void *)0,&TOUCH_TASK_STK[TOUCH_STK_SIZE-1],TOUCH_TASK_PRIO);
	OSTaskCreate(led_task,(void *)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();
}
void led_task(void *pdata)
{
	while(1)
	{
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500);
	}
}
void touch_task(void *pdata)
{
	while(1)
	{
		GUI_TOUCH_Exec();
		OSTimeDlyHMSM(0,0,0,10);
	}
}
void emwin_demo_task(void *pdata)
{
	while(1)
	{
//		GUIDEMO_Main();
		OSTimeDlyHMSM(0,0,0,10);
	}
}

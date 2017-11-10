#include  "systemInit.h"


//  定义LED
#define  LED_PERIPH             SYSCTL_PERIPH_GPIOG
#define  LED_PORT               GPIO_PORTG_BASE
#define  LED_PIN                GPIO_PIN_2


//  主函数（程序入口）
int main(void)
{
    jtagWait();                                             //  防止JTAG失效，重要！
    clockInit();                                            //  时钟初始化：晶振，6MHz

    SysCtlPeriEnable(LED_PERIPH);                           //  使能LED所在的GPIO端口
    GPIOPinTypeOut(LED_PORT, LED_PIN);                      //  设置LED所在管脚为输出

    for (;;)
    {
        GPIOPinWrite(LED_PORT, LED_PIN, 0x00);              //  点亮LED
        SysCtlDelay(150 * (TheSysClock / 3000));            //  延时约150ms

        GPIOPinWrite(LED_PORT, LED_PIN, 0xFF);              //  熄灭LED
        SysCtlDelay(850 * (TheSysClock / 3000));            //  延时约850ms
    }
}


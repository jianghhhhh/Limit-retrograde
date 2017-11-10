#include  "systemInit.h"


//  ����LED
#define  LED_PERIPH             SYSCTL_PERIPH_GPIOG
#define  LED_PORT               GPIO_PORTG_BASE
#define  LED_PIN                GPIO_PIN_2


//  ��������������ڣ�
int main(void)
{
    jtagWait();                                             //  ��ֹJTAGʧЧ����Ҫ��
    clockInit();                                            //  ʱ�ӳ�ʼ��������6MHz

    SysCtlPeriEnable(LED_PERIPH);                           //  ʹ��LED���ڵ�GPIO�˿�
    GPIOPinTypeOut(LED_PORT, LED_PIN);                      //  ����LED���ڹܽ�Ϊ���

    for (;;)
    {
        GPIOPinWrite(LED_PORT, LED_PIN, 0x00);              //  ����LED
        SysCtlDelay(150 * (TheSysClock / 3000));            //  ��ʱԼ150ms

        GPIOPinWrite(LED_PORT, LED_PIN, 0xFF);              //  Ϩ��LED
        SysCtlDelay(850 * (TheSysClock / 3000));            //  ��ʱԼ850ms
    }
}


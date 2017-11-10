#ifndef  __SYSTEM_INIT_H__
#define  __SYSTEM_INIT_H__


//  ������Ҫ��ͷ�ļ�
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"


//  ���ϳ��ı�ʶ������ɽ϶̵���ʽ
#define  SysCtlPeriEnable       SysCtlPeripheralEnable
#define  SysCtlPeriDisable      SysCtlPeripheralDisable
#define  GPIOPinTypeIn          GPIOPinTypeGPIOInput
#define  GPIOPinTypeOut         GPIOPinTypeGPIOOutput
#define  GPIOPinTypeOD          GPIOPinTypeGPIOOutputOD


//  ����ȫ�ֵ�ϵͳʱ�ӱ���
extern unsigned long TheSysClock;


//  ��ֹJTAGʧЧ
extern void jtagWait(void);


//  ϵͳʱ�ӳ�ʼ��
extern void clockInit(void);


#endif  //  __SYSTEM_INIT_H__


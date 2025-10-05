#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#include <stdint.h>

#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif

class System {
	static const uint32_t SystecmClockHz = 100000000UL;
public:
	System();
	void initClock();
	void initDma(uint32_t *timerCcData, uint32_t timerCcDataLen);
	void initTimerMain();
	void initUsart1();
	void initGpio();
	void runTimerMain(bool en);
	void updateTimerMainCompare(uint32_t compareValue);
	void updateTimerMainCompareNoPreload(uint32_t compareValue);
	void delay(uint32_t delayMs);
	void ledToggle();
	void ledOn(bool on = true);
	void ledOff(bool off = true);
};

#endif // __SYSTEM_H_

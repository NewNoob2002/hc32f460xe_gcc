//#include "lvgl.h"
#include <hc32_ddl.h>
#include "delay.h"
#ifndef USE_FREERTOS

#ifndef SYSTICK_TICK_FREQ
#  define SYSTICK_TICK_FREQ     1000 // Hz
#endif

#define SYSTICK_TICK_STEP (1000 / SYSTICK_TICK_FREQ)
#define SYSTICK_TICK_LOAD (SystemCoreClock/ SYSTICK_TICK_FREQ)
#define CYCLES_PER_MICROSECOND (SystemCoreClock/ 1000000)

#define SYSTICK_TICK_MS (SYSTICK_TICK_STEP * systick_ticks)

static volatile uint32_t systick_ticks = 0;
/*
 * @brief SysTick Init
 * @param none
 * @return none
 */
void delay_init() {
	SysTick_Config(SYSTICK_TICK_LOAD);
	NVIC_SetPriority(SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
}
/*
 * @brief SysTick Handler
 * @param none
 * @return none
 */
void SysTick_Handler() {
	systick_ticks += SYSTICK_TICK_STEP;
//	lv_tick_inc(1);
}

uint32_t millis()
{
	return SYSTICK_TICK_MS;
}

uint32_t micros(void)
{
	return (SYSTICK_TICK_MS * 1000 + (SYSTICK_TICK_LOAD - SysTick->VAL) / CYCLES_PER_MICROSECOND);
}

/**
  * @brief  毫秒级延时
  * @param  ms: 要延时的毫秒数
  * @retval 无
  */
void delay_ms(const uint32_t ms)
{
	const uint32_t start = systick_ticks;
	const uint32_t wait = ms / SYSTICK_TICK_STEP;

	while((systick_ticks - start) < wait)
	{
	}
}

/**
  * @brief  微秒级延时
  * @param  us: 要延时的微秒数
  * @retval 无
  */
void delay_us(const uint32_t us)
{
	uint32_t total = 0;
	const uint32_t target = CYCLES_PER_MICROSECOND * us;
	int last = SysTick->VAL;
	int now = last;
	int diff = 0;
	start:
		now = SysTick->VAL;
	diff = last - now;
	if(diff > 0)
	{
		total += diff;
	}
	else
	{
		total += diff + SYSTICK_TICK_LOAD;
	}
	if(total > target)
	{
		return;
	}
	last = now;
	goto start;
}

#else
//do nothing
#endif


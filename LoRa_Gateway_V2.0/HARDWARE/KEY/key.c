#include "key.h"
#include "led.h"
#include "tim.h"

extern TIM_HandleTypeDef htim6;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_5) {
	
		if (KEY0 == 0) { /* 按键被按下,启动定时器6，延时50ms 消抖 */
			TIM_Set(&htim6,1);
		}
		
	}
}
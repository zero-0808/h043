#ifndef _LED_H_
#define _LED_H_
#include "main.h"

//#define LED_R_Pin GPIO_PIN_3
//#define LED_R_GPIO_Port GPIOE
//#define LED_G_Pin GPIO_PIN_14
//#define LED_G_GPIO_Port GPIOG


#define LED_RED			{HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,GPIO_PIN_RESET);HAL_GPIO_WritePin(LED_G_GPIO_Port,LED_G_Pin,GPIO_PIN_SET);}
#define LED_GREEN		{HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,GPIO_PIN_SET);HAL_GPIO_WritePin(LED_G_GPIO_Port,LED_G_Pin,GPIO_PIN_RESET);}
#define LED_YELLOW		{HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,GPIO_PIN_RESET);HAL_GPIO_WritePin(LED_G_GPIO_Port,LED_G_Pin,GPIO_PIN_RESET);}
#define LED_OFF			{HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,GPIO_PIN_SET);HAL_GPIO_WritePin(LED_G_GPIO_Port,LED_G_Pin,GPIO_PIN_SET);}


void led_init(void);
uint8_t set_led_color(uint8_t color);
uint8_t set_led_mode(uint8_t mode);
uint8_t set_led_time(uint16_t time);
void led_work_state(uint8_t state);










#endif 



#ifndef __TIMER_H
#define __TIMER_H

    /****************   外部头文件声明   ****************/
    #include "sys.h"
    #include "usart.h"



    /****************    函数外部声明   *****************/
    void TIM6_Init(uint16_t arr, uint16_t psc);      /* 定时器6初始化 */
    void TIM7_Init(uint16_t arr, uint16_t psc);      /* 定时器7初始化 */

#endif

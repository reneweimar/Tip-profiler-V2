//-----------------------------------------------------------------------------
//! \file       indexmotor.h
//! \author     R. Weimar
//! \brief      Contains routines for the index motor
//! \details    
//! \Attention  
//-----------------------------------------------------------------------------
#ifndef _IDX_FUNCTIONS_H
#define _IDX_FUNCTIONS_H
//-----------------------------------------------------------------------------
#include "stm32f1xx_hal.h"
//-----------------------------------------------------------------------------
#define FaultIdx_Pin GPIO_PIN_6
#define FaultIdx_GPIO_Port GPIOC
#define SleepIdx_Pin GPIO_PIN_15
#define SleepIdx_GPIO_Port GPIOB
#define IntIndex_Pin GPIO_PIN_0
#define IntIndex_GPIO_Port GPIOA
#define IntIndex_EXTI_IRQn EXTI0_IRQn
#define A1_Pin GPIO_PIN_14
#define A1_GPIO_Port GPIOB
#define A2_Pin GPIO_PIN_13
#define A2_GPIO_Port GPIOB
#define A_Pin GPIO_PIN_8
#define A_GPIO_Port GPIOA
#define B_Pin GPIO_PIN_11
#define B_GPIO_Port GPIOA
#define A1On()HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin,GPIO_PIN_SET)
#define A1Off()HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin,GPIO_PIN_RESET)
#define A2On()HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin,GPIO_PIN_SET)
#define A2Off()HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin,GPIO_PIN_RESET)
#define IDX_Enable() HAL_GPIO_WritePin(SleepIdx_GPIO_Port, SleepIdx_Pin,GPIO_PIN_SET)
#define IDX_Disable() HAL_GPIO_WritePin(SleepIdx_GPIO_Port, SleepIdx_Pin,GPIO_PIN_RESET)

//-----------------------------------------------------------------------------
//! \brief  Index motor structure
typedef struct
{
    uint8_t StepsPerRevolution;   //200
    uint8_t NrOfSteps;            //4, bistable motor, so 2 phases, so 4 steps
    int8_t CurrentStep;           //0 to 3
    uint16_t StepDelay;
    int32_t SetPosition;
    int32_t GetPosition;
} stcStepperMotor;
//-----------------------------------------------------------------------------
//IDX_functions
//---------------------- SYSTEM ------------------------
extern stcStepperMotor gIndexMotor;

void IDX_Init(void);
void IDX_Tick(void);

#endif  // _IDX_FUNCTIONS_H


//-----------------------------------------------------------------------------
//! \file       strokemotor.h
//! \author     R. Weimar
//! \brief      Contains routines for the stroke motor
//! \details
//! \Attention
//-----------------------------------------------------------------------------
#ifndef _STR_FUNCTIONS_H
#define _STR_FUNCTIONS_H
//-----------------------------------------------------------------------------
#define IntEncoder_Pin GPIO_PIN_1
#define IntEncoder_GPIO_Port GPIOA
#define IntHome_Pin GPIO_PIN_2
#define IntHome_GPIO_Port GPIOA
#define SleepStr_Pin GPIO_PIN_5
#define SleepStr_GPIO_Port GPIOC
#define MotRev_Pin GPIO_PIN_0
#define MotRev_GPIO_Port GPIOB
#define MotFwd_Pin GPIO_PIN_1
#define MotFwd_GPIO_Port GPIOB
#define FaultStr_Pin GPIO_PIN_2
#define FaultStr_GPIO_Port GPIOB
//-----------------------------------------------------------------------------
//! \brief  Index motor structure
typedef struct
{
    uint8_t PulsesPerRevolution;   //12
    uint16_t GetSpeed;
    uint16_t SetSpeed;
    uint32_t Encoder;
    uint16_t TimerValue;
    uint16_t TimerValueOld;
    uint16_t TimePerRev;
} stcDCMotor;

extern stcDCMotor gSTR_Motor;
//-----------------------------------------------------------------------------
//STR_functions
//---------------------- SYSTEM ------------------------
extern void STR_Init(void);
extern void STR_HandleEncoder (void);

#endif  // _STR_FUNCTIONS_H


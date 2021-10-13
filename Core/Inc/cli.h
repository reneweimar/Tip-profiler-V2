//-----------------------------------------------------------------------------
//! \file       cli.h
//! \author     R. Weimar
//! \brief      Contains routines for the cli handling
//! \details    
//! \Attention  
//-----------------------------------------------------------------------------
#ifndef _CLI_DEFINITION_H
#define _CLI_DEFINITION_H
//-----------------------------------------------------------------------------
#include "stm32f1xx_hal.h"
//-----------------------------------------------------------------------------
//CLI_functions
//---------------------- SYSTEM ------------------------
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void CLI_Init (void);

#endif  // _CLI_DEFINITION_H


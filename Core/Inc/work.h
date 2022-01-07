////-----------------------------------------------------------------------------
//! \file       work.h
//! \author     R. Weimar
//! \brief      Contains routines for the general sequence
//! \details    
//! \Attention  
//-----------------------------------------------------------------------------
#ifndef _WRK_FUNCTIONS_H
#define _WRK_FUNCTIONS_H
//-----------------------------------------------------------------------------
#include "stm32f1xx_hal.h"
//-----------------------------------------------------------------------------
#define SPLASHSCREENTIME        3000
#define VERSIONMAJOR            1
#define VERSIONMINOR            1
#define LOWPOWERTIME            10000
#define LOWCONTRAST             100
#define HIGHCONTRAST            255
#define NROFMACHINETYPES        4
#define NB_OF_VAR               20*NROFMACHINETYPES //(20 per machine type)
#define SCREENSAVERON           gMachineType[gMachine/100].Parameters[4].Value==100

//-----------------------------------------------------------------------------
//! \brief  Status enumeration
typedef enum 
{
    UNDEFINED=0,
    ACTIVE,
    CCW,
    CW,
    ENCODERTEST,
    ENTERVALUE,
    EXECUTECOMMAND,
    GOTOPOSITION,
    GOTOSTARTPOSITION,
    HOME,
    INACTIVE,
    INITIALIZE,
    READY,
    SCRAPEREED,
    SCRAPEINNERSECTIONS,
    SCRAPENOSIDESTEPS,
    SCRAPEOUTERSECTIONS,
    START,
    STOP,
    WAITFORCOMMANDFINISHED,
    WAITFORHOMESENSOR,
    WAITFORINDEXHOME,
    WAITFORINDEXSTART,
    WAITFORPOSITION,
    WAITFORSCRAPEREADY,
    WAITFORSPLASHSCREEN,
    WAITFORSTROKEMOTORHOME,
    WAITFORSTROKEMOTORSTART,
    WAITFORSTROKEMOTORSTOP,
    WAITFORUSER,
}enuStatus;
typedef enum
{
  TASK_IDLE=0,
  TASK_BUSY,
  TASK_ERROR,
  TASK_READY
} enuTask;
//-----------------------------------------------------------------------------
//! \brief  Status type enumeration
typedef enum
{
    MainStatus,
    MainStatusOld,
    SubStatus,
    SubStatusOld,
} enuType;
//-----------------------------------------------------------------------------
//! \brief  Status storage structure
typedef struct
{
    enuStatus MainStatus;
    enuStatus MainStatusOld;
    enuStatus SubStatus;
    enuStatus SubStatusOld;
} stcStatus;
//-----------------------------------------------------------------------------
//! \brief  Parameter storage structure
typedef struct
{
  char Name[21];
  int16_t Min;
  int16_t Max;
  int16_t Value;
  char Unit[10];
  uint8_t Digits;
  uint8_t Decimals;
  uint8_t UserAccess;
  uint8_t Global;
  uint16_t MasterCounter;
  uint16_t ServiceCounter;
} StcParameters;
//-----------------------------------------------------------------------------
//! \brief  Parameter storage structure
typedef struct
{
  char Name[21];
  uint8_t UserAccess;
} StcCommands;
//-----------------------------------------------------------------------------
//! \brief  Machinetype storage structure
typedef struct
{
  char Name[21];  //OBOE, BASSOON, future -> KLARINET
  StcParameters Parameters[20];
} StcMachine;
//-----------------------------------------------------------------------------
//! \brief counter Storage structure
typedef struct
{
    uint32_t Sequence;
    uint32_t User; //Counter to check user action
    uint16_t Delay;
} stcCounter;
//-----------------------------------------------------------------------------
//! \brief  Scrape step enumeration
typedef enum 
{
  RightSideNormalStep = 0,
  RightSideLastStep = 1,
  RightSideLastScrape = 2,
  RightSideEndOfScraping = 3,
  LeftSideNormalStep = 4,
  LeftSideLastStep = 5,
  LeftSideLastScrape = 6,
  LeftSideEndOfScraping = 7,
  RightSidePauseRequested = 8,
  RightSidePaused = 9,
  LeftSidePauseRequested = 10,
  LeftSidePaused = 11,
   
} enuScrapeStatus;

//! \brief scrape Storage structure
typedef struct
{
  enuScrapeStatus Status;
  enuScrapeStatus StatusOld;
  enuScrapeStatus StatusPause;
  uint8_t NextSideStep;
  uint8_t NextScrape;
  int32_t StartPosition;
  int32_t EndPosition;
  int32_t SideStep;
  int32_t Speed;
} stcScrape;
//-----------------------------------------------------------------------------
//! \brief  Public variables
extern uint8_t gInitialized;
extern uint8_t PluggedIn;
extern uint8_t gServiceMenu;
extern uint8_t gParameterMaxUser;
extern uint8_t BattPercentage;
extern uint16_t gMachine;
extern uint16_t gParameterMaxService;
extern uint8_t gCommandMaxUser;
extern uint16_t gCommandMaxService;
extern uint16_t VirtAddVarTab[NB_OF_VAR];
extern uint16_t ADC_Converted_Values[1];
extern uint32_t HomeCnt;
extern uint32_t HomeCntDelay;
extern int32_t gIDX_HalfScrapeWidth;
extern int32_t gIDX_SideStepBig;
extern int32_t gIDX_SideStepSmall;
extern uint8_t gIDX_StatusFlag;
extern uint8_t gSTR_NextSideStep;
extern stcScrape gScrape;
extern stcStatus gWRK_Status;
extern StcMachine gMachineType[NROFMACHINETYPES];
extern StcCommands gCommands[];
extern stcCounter gCounter;
//-----------------------------------------------------------------------------
//WRK_functions
//---------------------- SYSTEM ------------------------
extern void SetScrapeStatus (enuScrapeStatus newStatus);
extern void WRK_HandleTickTime (void);
extern void WRK_HandleSequence(void);
extern void WRK_ResetFactory(void);
extern void WRK_Init(void);
//-----------------------------------------------------------------------------
#endif  // _WRK_FUNCTIONS_H


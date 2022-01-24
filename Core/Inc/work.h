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
#define VERSIONTWEAK            23
#define LOWPOWERTIME            10000
#define NOBATTERYCONTRAST       25
#define LOWCONTRAST             75
#define HIGHCONTRAST            255
#define NROFMACHINETYPES        4
#define NROFPARAMETERS          20
#define NROFCOUNTERS            2
#define NROFERRORS              100
#define NROFMAINMENUITEMS       5
#define NB_OF_VAR               NROFPARAMETERS*NROFMACHINETYPES+NROFERRORS+NROFCOUNTERS*2//(20 per machine type)+ 100 error messages+ 2 counters LSB and MSB
#define SCREENSAVERON           gMachineType[gMachine/100].Parameters[SCREENSAVER].Value==100
#define SIDESTEPDIFFERENCE      5
#define SCRAPEWIDTHDIFFERENCE   100


//-----------------------------------------------------------------------------
//! \brief  Status enumeration
typedef enum 
{
    UNDEFINED=0,
    ACTIVE,
    CCW,
    CW,
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
    UNITERROR,
    WAITFORCOMMANDFINISHED,
    WAITFORHOMESENSOROFF,
    WAITFORHOMESENSORON,
    WAITFORHOMESENSORON2,
    WAITFORINDEXHOME,
    WAITFORINDEXSTART,
    WAITFORPLUGIN,
    WAITFORPOSITION,
    WAITFORPOWERSENSOR,
    WAITFORSCRAPEREADY,
    WAITFORSPLASHSCREEN,
    WAITFORSTROKEMOTORHOME,
    WAITFORSTROKEMOTORSTART,
    WAITFORSTROKEMOTORSTOP,
    WAITFORUSER,
    WAITFORUSER2,
}enuStatus;
typedef enum //Numbering cannot be changed!!!!
{
  SCRAPEWIDTH = 0,
  SCRAPEWIDTHINNER = 1,
  SCRAPESPEED = 2,
  SIDESTEPSMALL = 3,
  SIDESTEPBIG = 4,
  SCREENSAVER = 5,  
  MACHINETYPE = 6,  
  SIDESTEPOFFSET = 7,
  SCRAPESPEEDMIN = 8,
  SCRAPESPEEDMAX = 9,
  SCRAPEWIDTHMAX = 10,
  SCRAPEWIDTHFACT = 11,
  SCRAPESPEEDFACT = 12,
  SIDESTSMALLFACT = 13,
  SIDESTBIGFACT = 14,
  SIDESTRATIO = 15,
} enuParameters;

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
} StcParameters;
//-----------------------------------------------------------------------------
//! \brief  Commands storage structure
typedef struct
{
  char Name[21];
  uint8_t UserAccess;
} StcCommands;
//-----------------------------------------------------------------------------
//! \brief  Machinetype storage structure
typedef struct
{
  char Name[21];  //OBOE, BASSOON, CLARINET, BAGPIPE
  StcParameters Parameters[20];
} StcMachine;
//-----------------------------------------------------------------------------
//! \brief  Main Menu storage structure
typedef struct
{
  char Name[21];  //OBOE, BASSOON, CLARINET, BAGPIPE
  uint8_t UserAccess;
} StcMainMenu;
//-----------------------------------------------------------------------------
//! \brief counter Storage structure
typedef struct
{
    uint32_t Sequence;
    uint32_t MasterCounter;
    uint32_t ServiceCounter;
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
  NoSideStep = 12,
  NoSideStepPauseRequested = 13,
  NoSideStepPaused = 14,
} enuScrapeStatus;

//! \brief scrape Storage structure
typedef struct
{
  enuScrapeStatus Status;
  enuScrapeStatus StatusOld;
  enuScrapeStatus StatusPause;
  uint8_t NextSideStep;
  uint8_t NextScrape;
  uint8_t Endless;
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
extern uint8_t gMainMenuMaxService;
extern uint8_t gMainMenuMaxUser;
extern uint8_t gCommandMaxUser;
extern uint16_t gCommandMaxService;
extern uint16_t VirtAddVarTab[NB_OF_VAR];
extern uint16_t ADC_Converted_Values[1];
extern uint32_t HomeCnt;
extern int32_t gIDX_HalfScrapeWidth;
extern int32_t gIDX_SideStepBig;
extern int32_t gIDX_SideStepSmall;
extern uint8_t gIDX_StatusFlag;
extern uint8_t gSTR_NextSideStep;
extern int8_t ErrorDisplayPage;
extern uint16_t Errors[NROFERRORS];
extern uint8_t WRK_UpdateContrast;
extern enuParameters gParameters;
extern StcMainMenu MainMenu[NROFMAINMENUITEMS];
extern stcScrape gScrape;
extern stcStatus gWRK_Status;
extern StcMachine gMachineType[NROFMACHINETYPES];
extern StcCommands gCommands[];
extern stcCounter gCounter;
//-----------------------------------------------------------------------------
//WRK_functions
//---------------------- SYSTEM ------------------------
//Local functions
void WRK_ShowError (uint16_t newError);
uint8_t WRK_CheckConditions(void);
void WRK_HandleActive(void);
void WRK_HandleBatteryStatus (void);
void WRK_HandleCommand(uint32_t newCommand);
void WRK_HandleContrast(void);
void WRK_HandleEnterValue(void);
void WRK_HandleExecuteCommand(void);
void WRK_HandleResetFactory(void);
void WRK_HandleInitialize(void);
void WRK_HandleScrapeReed (void);
void WRK_HandleSensors(void);
void WRK_SetStatus (enuType newType, enuStatus newStatus);
void WRK_ShowError (uint16_t newError);
//Exported functions
extern void WRK_HandleSequence(void);
extern void WRK_HandleTickTime (void);
extern void WRK_Init(void);
extern void WRK_ResetFactory(void);
extern void WRK_SetScrapeStatus (enuScrapeStatus newStatus);
//-----------------------------------------------------------------------------
#endif  // _WRK_FUNCTIONS_H


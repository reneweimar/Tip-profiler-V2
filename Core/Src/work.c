//-----------------------------------------------------------------------------
//! \file       work.c
//! \author     R. Weimar
//! \brief      Contains routines for the general sequence
//! \details    
//! \Attention  
//-----------------------------------------------------------------------------
#include "main.h"
#include "work.h"
#include "string.h"
#include "eeprom.h"
#include "userinterface.h"
#include "power.h"
#include "adc.h"
#include "strokemotor.h"
#include "indexmotor.h"
#include "i2c.h"
#include "ssd1306.h"
#include <stdlib.h>
//-----------------------------------------------------------------------------
//! \Side step size
uint16_t WRK_SideStep;
//! \Global ADC result
uint16_t ADC_Converted_Values[1];
//! \Global scrape control structure
stcScrape gScrape;
//! \Global main and sub, current and previous status of the device
stcStatus gWRK_Status;
//! \Global counter structure
stcCounter gCounter;
//! \Global Initialization flag
uint8_t gInitialized = 0;
//! \Global temp battery level (Replace by ADC)
uint8_t BattPercentage = 50;
//! \Global machine type 0 (oboe), type 1 (Bassoon)
uint16_t gMachine;
//! \Global Command to repeat
uint8_t gRepeatCommand;
//! \Global parameters container defaults for machine type 0 (oboe)
StcMachine gMachineType[NROFMACHINETYPES];
//! \Global commands container
StcCommands gCommands[] =
{
  {"RESET TO FACTORY   ",1},
  {"SET STROKE LENGTH  ",1},
  {"STROKE MOTOR       ",0},
};
//! \Global User command max flag
uint8_t gCommandMaxUser;
//! \Global Service command max flag
uint16_t gCommandMaxService = sizeof(gCommands)/sizeof(gCommands[0])-1;
//! \Global parameters container
StcParameters DefaultsMachine[NROFMACHINETYPES][20]=
{ //OBOE
	{ 
    {"SCRAPE WIDTH      ",0,1160,780,"MM",3,1,1,0},
    {"SCRAPE WIDTH INNER",0,200,390,"MM",3,1,1,0},
    {"SCRAPE SPEED      ",50,200,50,"RPS",2,1,1,0},
    {"SIDE STEP SMALL   ",5,50,30,"MM",3,2,1,0},
    {"SIDE STEP BIG     ",30,100,50,"MM",3,2,1,0},
    {"SCREEN SAVER      ",0,100,0,"-",1,0,0,1},
    {"MACHINE TYPE      ",0,100,0,"-",1,0,0,1},
    {"SIDE STEP OFFSET  ",-100,100,0,"MM",3,2,0,0},
    {"SCRAPE SPEED MIN  ",10,100,10,"RPS",2,1,0,0},
    {"SCRAPE SPEED MAX  ",10,200,200,"RPS",2,1,0,0},
    {"SCRAPE WIDTH MAX  ",0,2500,1160,"MM",3,1,0,0},
    {"SCRAPE WIDTH FACT ",0,2500,780,"MM",3,1,0,0},
    {"SCRAPE SPEED FACT ",0,200,200,"RPS",1,1,0,0},
    {"SIDE ST SMALL FACT",0,500,30,"MM",3,2,0,0},
    {"SIDE ST BIG FACT  ",0,500,30,"MM",3,2,0,0},
	},
	{ //BASSOON
    {"SCRAPE WIDTH      ",0,2400,1660,"MM",3,1,1,0},
    {"SCRAPE WIDTH INNER",0,200,830,"MM",3,1,1,0},
    {"SCRAPE SPEED      ",50,200,50,"RPS",2,1,1,0},
    {"SIDE STEP SMALL   ",5,60,40,"MM",3,2,1,0},
    {"SIDE STEP BIG     ",40,100,60,"MM",3,2,1,0},
    {"SCREEN SAVER      ",0,100,0,"-",1,0,0,1},
    {"MACHINE TYPE      ",0,100,0,"-",1,0,0,1},
    {"SIDE STEP OFFSET  ",-100,100,0,"MM",3,2,0,0},
    {"SCRAPE SPEED MIN  ",10,100,10,"RPS",2,1,0,0},
    {"SCRAPE SPEED MAX  ",10,200,200,"RPS",2,1,0,0},
    {"SCRAPE WIDTH MAX  ",0,2500,2400,"MM",3,1,0,0},
    {"SCRAPE WIDTH FACT ",0,2500,1160,"MM",3,1,0,0},
    {"SCRAPE SPEED FACT ",0,200,200,"RPS",1,1,0,0},
    {"SIDE ST SMALL FACT",0,500,40,"MM",3,2,0,0},
    {"SIDE ST BIG FACT  ",0,500,40,"MM",3,2,0,0},
	},
	{ //KLARINET
    {"SCRAPE WIDTH      ",0,2400,1660,"MM",3,1,1,0},
    {"SCRAPE WIDTH INNER",0,200,830,"MM",3,1,1,0},
    {"SCRAPE SPEED      ",50,200,50,"RPS",2,1,1,0},
    {"SIDE STEP SMALL   ",5,60,40,"MM",3,2,1,0},
    {"SIDE STEP BIG     ",40,100,60,"MM",3,2,1,0},
    {"SCREEN SAVER      ",0,100,0,"-",1,0,0,1},
    {"MACHINE TYPE      ",0,100,0,"-",1,0,0,1},
    {"SIDE STEP OFFSET  ",-100,100,0,"MM",3,2,0,0},
    {"SCRAPE SPEED MIN  ",10,100,10,"RPS",2,1,0,0},
    {"SCRAPE SPEED MAX  ",10,200,200,"RPS",2,1,0,0},
    {"SCRAPE WIDTH MAX  ",0,2500,2400,"MM",3,1,0,0},
    {"SCRAPE WIDTH FACT ",0,2500,1160,"MM",3,1,0,0},
    {"SCRAPE SPEED FACT ",0,200,200,"RPS",1,1,0,0},
    {"SIDE ST SMALL FACT",0,500,40,"MM",3,2,0,0},
    {"SIDE ST BIG FACT  ",0,500,40,"MM",3,2,0,0},
	},
  { //BAGPIPE
    {"SCRAPE WIDTH      ",0,2400,1660,"MM",3,1,1,0},
    {"SCRAPE WIDTH INNER",0,200,830,"MM",3,1,1,0},
    {"SCRAPE SPEED      ",50,200,50,"RPS",2,1,1,0},
    {"SIDE STEP SMALL   ",5,60,40,"MM",3,2,1,0},
    {"SIDE STEP BIG     ",40,100,60,"MM",3,2,1,0},
    {"SCREEN SAVER      ",0,100,0,"-",1,0,0,1},
    {"MACHINE TYPE      ",0,100,0,"-",1,0,0,1},
    {"SIDE STEP OFFSET  ",-100,100,0,"MM",3,2,0,0},
    {"SCRAPE SPEED MIN  ",10,100,10,"RPS",2,1,0,0},
    {"SCRAPE SPEED MAX  ",10,200,200,"RPS",2,1,0,0},
    {"SCRAPE WIDTH MAX  ",0,2500,2400,"MM",3,1,0,0},
    {"SCRAPE WIDTH FACT ",0,2500,1160,"MM",3,1,0,0},
    {"SCRAPE SPEED FACT ",0,200,200,"RPS",1,1,0,0},
    {"SIDE ST SMALL FACT",0,500,40,"MM",3,2,0,0},
    {"SIDE ST BIG FACT  ",0,500,40,"MM",3,2,0,0},
  },
};
//! \Global Service flag
uint8_t gServiceMenu;
//! \Global User parameter max flag
uint8_t gParameterMaxUser;
//! \Global Service parameter max flag
uint16_t gParameterMaxService = 14; //14 is last parameter
//! \Global virtual tab for eeprom simulation 0-99 for machine0, 100-199 for machine1, etc
uint16_t VirtAddVarTab[NB_OF_VAR] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119};

uint16_t ParameterTemp[20];

uint32_t HomeCnt,HomeCntDelay;


float BatteryVoltage;
//-----------------------------------------------------------------------------
//! \brief      Initiates the work unit
//! \details    Defines the parameters
//! \param      None
void WRK_ResetFactory(void)
{
  for (uint8_t i = 0; i < NROFMACHINETYPES; i++)
  {
    memcpy( &gMachineType[i].Parameters, &DefaultsMachine[i], sizeof DefaultsMachine[i]);
    for (uint8_t j=0; j<gParameterMaxService ;j++) //Check eeprom memory
    {
      EE_WriteVariable(i+j, gMachineType[i].Parameters[j].Value);
    }
  }
}

//-----------------------------------------------------------------------------
//! \brief      Initiates the work unit
//! \details    Defines the parameters
//! \param      None
void WRK_Init(void)
{
  for (uint8_t i = 0; i < NROFMACHINETYPES; i++)
  {
    memcpy( &gMachineType[i].Parameters, &DefaultsMachine[i], sizeof DefaultsMachine[i]);
  }
  strcpy(gMachineType[0].Name, "OBOE");
  strcpy(gMachineType[1].Name, "BASSOON");
  strcpy(gMachineType[2].Name, "CLARINET");
  strcpy(gMachineType[3].Name, "BAGPIPE");
    
  EE_ReadVariable(6, &gMachine);
  
  uint8_t NotEmpty = 0;

  //uint16_t ParameterTemp[gParameterMaxService];
  for (uint8_t j=0; j<NROFMACHINETYPES; j++)
	{
		for (uint8_t i=0; i<gParameterMaxService ;i++) //Check eeprom memory
		{
			EE_ReadVariable(i + j * 100, &ParameterTemp[i]);
			if ((ParameterTemp[i]>0) && (i != 6)) NotEmpty = 1;
		}
		if (NotEmpty == 1) //Copy new values over gParameter
		{
			for (uint8_t i=0; i<gParameterMaxService ;i++) //Check eeprom memory
			{
				gMachineType[j].Parameters[i].Value = ParameterTemp[i];
			}
		}
		else //Store default values from gParameter
		{
			for (uint8_t i=0; i<gParameterMaxService ;i++) //Check eeprom memory
			{
				EE_WriteVariable(i+ j * 100, gMachineType[j].Parameters[i].Value);
			}
		}
	}
  for (uint8_t i=1; i<gParameterMaxService ;i++) //Always assume at least 1 User parameter
  {
    if (gMachineType[gMachine/100].Parameters[i].UserAccess == 1)
    {
      gParameterMaxUser = i;
    }
  }
  for (uint8_t i=1; i<gCommandMaxService ;i++) //Always assume at least 1 User parameter
  {
    if (gCommands[i].UserAccess == 1)
    {
      gCommandMaxUser = i;
    }
  }

}

//-----------------------------------------------------------------------------
//! \brief      Sets the global status
//! \details    Sets the selected status (main or sub) and stores the 
//! previous status
//! \param[in]  newType   MainStatus or SubStatus
//! \param[in]  newStatus New status for the selected status
void WRK_SetStatus (enuType newType, enuStatus newStatus)
{
    if (newType == MainStatus)
    {
        gWRK_Status.MainStatusOld = gWRK_Status.MainStatus;
        gWRK_Status.MainStatus = newStatus;
        gWRK_Status.SubStatusOld = gWRK_Status.SubStatus;
        gWRK_Status.SubStatus = UNDEFINED;
    }
    else
    {
        gWRK_Status.SubStatusOld = gWRK_Status.SubStatus;
        gWRK_Status.SubStatus = newStatus;
    }
}

//-----------------------------------------------------------------------------
//! \brief      Handles the battery status
//! \details    Handles 1 ms time interrupt 
//! \param      None
void WRK_HandleBatteryStatus (void)
{
    static uint16_t TickTime = 0; 
    static uint8_t ConvertedValueOld;
    if (TickTime++ < 499) return; //4997ms 
    TickTime = 0;

    //if (gWRK_Status.SubStatus == WAITFORUSER) 
    //{ 
      if (ConvertedValueOld != ADC_Converted_Values[0])
      {
        BatteryVoltage = (uint16_t) ((float) ADC_Converted_Values[0] / 4095 * 4210); //4210 = 3300 * (9100 + 33000) / 33000
        if (BatteryVoltage <= BATTVOLTAGEMIN)
          BattPercentage = 0;  
        else if (BatteryVoltage>= BATTVOLTAGEMAX)
          BattPercentage = 100;  
        else
          BattPercentage = (uint8_t) (((BatteryVoltage - BATTVOLTAGEMIN)/(BATTVOLTAGEMAX-BATTVOLTAGEMIN))* 100 );
        ConvertedValueOld = ADC_Converted_Values[0];
        USR_ShowBattery(BattPercentage);
      }
    //}
}
//-----------------------------------------------------------------------------
//! \brief      Handles the tick time counter
//! \details    Handles 1 ms time interrupt 
//! \param      None
void WRK_HandleTickTime (void)
{
  if (gInitialized)
  {
    gCounter.Delay ++;
    WRK_HandleSequence();
    WRK_HandleBatteryStatus();
    USR_HandleButtons();
    IDX_HandleMotor();
    STR_HandleMotor();
  }
}

//-----------------------------------------------------------------------------
//! \brief      Handles the command
//! \details    Handles actions to form th emain sequence 
//! \param[in]  uint16_t newCommand
//! \param[in]  uint16_t LastScreen
void WRK_HandleCommand(uint32_t newCommand)
{
	newCommand = (gCurrentScreen - 1020000) * 10 + newCommand;
  switch (newCommand)
  {
    case 1010: //Cancel, go back
		case 2010: //Cancel, go back
	  case 3010: //Cancel, go back
    {
      USR_ShowScreen (gCurrentScreen / 100); 
      WRK_SetStatus(MainStatus,ACTIVE);
      WRK_SetStatus(SubStatus,WAITFORUSER);
      break;
    }
    case 1011: //Reset factory OK
    {
      WRK_ResetFactory();
      USR_ShowScreen (gCurrentScreen / 100); 
      WRK_SetStatus(MainStatus,ACTIVE);
      WRK_SetStatus(SubStatus,WAITFORUSER);
      break;
    } 
    case 2011: //Set stroke length: OK -> INDEX HOME Position -> STROKE HOME position
    {
      USR_ShowScreen (gCurrentScreen + 1);   
      WRK_SetStatus(MainStatus, ACTIVE);
      WRK_SetStatus(SubStatus, WAITFORINDEXHOME);
			break;
		}
		case 2031: //Second time OK -> Goto START and menu up
		{
      USR_ShowScreen (gCurrentScreen + 1);   
			WRK_SetStatus(MainStatus, ACTIVE);
			WRK_SetStatus(SubStatus, WAITFORSTROKEMOTORSTART);
			break;
    }
    case 32: //Stroke motor on
    {
      break;
    }
    case 33: //Stroke motor off
    {
      break;
    }
    case 34: //Stroke motor faster
    {
      break;
    }
    case 35: //Stroke motor slower
    {
      break;
    }
    default:
      break;
  }
}  

//-----------------------------------------------------------------------------
//! \brief      Handles the sequence
//! \details    Handles actions to form the emain sequence 
//! \param      None
void WRK_HandleSequence(void)
{
  static uint8_t TickTime = 0; 
  static uint16_t LastScreen = 0;
  static uint16_t LastNormalScreen = 0;
  static uint8_t CntStep;
  
  if (TickTime++ < 99) return; //100ms 
  TickTime = 0;

  gCounter.Sequence += 100;
  if (HomeCntDelay) HomeCntDelay -= 100;
  
  if (PluggedIn())
  {
    gCounter.User = 0;
    ssd1306_SetContrast(HIGHCONTRAST);
  }
  else 
  {
    if (gCounter.User < 0xffffffff) gCounter.User += 100;
    if ((gCounter.User > LOWPOWERTIME)&&(SCREENSAVERON)) 
    {
      ssd1306_SetContrast(LOWCONTRAST);
    }
  }
  
    
  switch (gWRK_Status.MainStatus)
  {
    case UNDEFINED:
    {
      WRK_SetStatus(MainStatus,INACTIVE);
      break;
    }
    case INACTIVE:
    {
      gCounter.Sequence = 0;
      USR_ShowScreen(1);
      WRK_SetStatus(MainStatus,ACTIVE);
      break;
    }
    case SCRAPEREED:
    {
      switch (gWRK_Status.SubStatus)
      {
        case UNDEFINED:
        {
          if (gIDX_Motor.IsHomed == 0)
          {
            gUSR_SetMessage("SIDE STEP SET IS","","NOT HOMED","","","OK");
            gReturnScreen = gCurrentScreen;
            USR_ShowScreen(3);
            WRK_SetStatus(MainStatus, ACTIVE);
            WRK_SetStatus(SubStatus, WAITFORUSER);
          }
          else if ((gIDX_Motor.IsInStartPosition == 0) || (gSTR_Motor.IsInStartPosition == 0))
          {
            gUSR_SetMessage("SIDE STEP SET OR","","STROKE SET NOT AT","","START POSITION","OK");
            gReturnScreen = gCurrentScreen;
            USR_ShowScreen(3);
            WRK_SetStatus(MainStatus, ACTIVE);
            WRK_SetStatus(SubStatus, WAITFORUSER);
          }
          else
          {
            gUSR_SetMessage("","SCRAPING","","OK: PAUSE SCRAPING","","OK");
            gReturnScreen = gCurrentScreen;
            USR_ShowScreen(4);
            gScrape.Status = 0; //Right side of the reed
            WRK_SetStatus(SubStatus, WAITFORPOSITION);
          }
          break;
        }
        case WAITFORPOSITION:
        {
          gScrape.Speed = gMachineType[gMachine].Parameters[2].Value*30; // Value / 100 * 50 (ratio) * 60 (1 min = 60 s)
          USR_ClearPosition();
          if (IDX_Set(GOTOPOSITION, gScrape.StartPosition )==READY)
          {
#ifdef IDX_SHOWREALPOSITION
              USR_ShowPosition((int32_t) ((float) gIDX_Motor.GetPosition / gIDX_Motor.UmPerPulse));
#else
              USR_ShowPosition((int32_t) ((float) gIDX_Motor.SetPosition / gIDX_Motor.UmPerPulse));
#endif
            IDX_Set(UNDEFINED,0);
            WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);
          }
          break;
        }
        case WAITFORSTROKEMOTORSTART:
        {
          if (STR_Set(START,gScrape.Speed) == READY) 
          {
            STR_Set(UNDEFINED,0);
            WRK_SetStatus(SubStatus, WAITFORSTROKEMOTORSTOP);
          }
          break;
        }
        case WAITFORSTROKEMOTORSTOP:
        {
          if (USR_ButtonPressed(BtnOk,USR_SHORTPRESSTIME,1)==1)
          {
            if (gScrape.Status == 0) //Busy scraping right part.
            {
              gScrape.Status = 6;
              gUSR_SetMessage("","","    PAUSE SCRAPING","","","");
              USR_ShowScreen(4);
              
              break;
            }
            else if (gScrape.Status == 3) //Busy scraping left part.
            {
              gScrape.Status = 8;
              gUSR_SetMessage("","","    PAUSE SCRAPING","","","");
              USR_ShowScreen(4);
              break;
            }
          }
          if (gScrape.Status == 2)
          {
            gIDX_Motor.MainStatus = (INACTIVE);
            IDX_Set(UNDEFINED,0);
            gScrape.Status = 3; 
            gScrape.EndPosition *= -1; //Left side of the reed
            gScrape.StartPosition *= -1; //Left side of the reed
            WRK_SetStatus(SubStatus, WAITFORPOSITION);            
          }
          else if (gScrape.Status == 5)
          {
            gIDX_Motor.MainStatus = (INACTIVE);
            IDX_Set(UNDEFINED,0);
            if (gScrape.EndPosition == 0)
            {
              WRK_SetStatus(MainStatus, ACTIVE);
              WRK_SetStatus(SubStatus, WAITFORUSER);
              USR_ShowScreen(gReturnScreen); 
            }
            else
            {
              USR_ClearPosition();
              WRK_SetStatus(SubStatus, WAITFORINDEXHOME);
            }
          }
          else if ((gScrape.Status == 7) || (gScrape.Status == 9))
          {
            gUSR_SetMessage("","OK: CONTINUE SCRAPING",""," *: CANCEL SCRAPING","","*OK");
            USR_ShowScreen(4);
            WRK_SetStatus(SubStatus,WAITFORUSER);
          }
          break;
        }
        case WAITFORINDEXHOME:
        {
          if (IDX_Set(GOTOPOSITION, 0)==READY)
          {
            IDX_Set(UNDEFINED,0);
            WRK_SetStatus(MainStatus, ACTIVE);
            WRK_SetStatus(SubStatus, WAITFORUSER);
            USR_ShowScreen(gReturnScreen); 
          }
        }
        case WAITFORUSER:
        {
          if (USR_ButtonPressed(BtnOk,USR_SHORTPRESSTIME,1)==1)
          {
            if (gScrape.Status == 7) //pause scraping right part.
            {
              gScrape.Status = 0;
              gUSR_SetMessage("","SCRAPING","","OK: PAUSE SCRAPING","","OK");
              USR_ShowScreen(4);
#ifdef IDX_SHOWREALPOSITION
                USR_ShowPosition((int32_t) ((float) gIDX_Motor.GetPosition / gIDX_Motor.UmPerPulse));
#else
                USR_ShowPosition((int32_t) ((float) gIDX_Motor.SetPosition / gIDX_Motor.UmPerPulse));
#endif
              WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);
              break;
            }
            else if (gScrape.Status == 9) //Pause scraping left part.
            {
              gScrape.Status = 3;
              gUSR_SetMessage("","SCRAPING","","OK: PAUSE SCRAPING","","OK");
              USR_ShowScreen(4);
#ifdef IDX_SHOWREALPOSITION
                USR_ShowPosition((int32_t) ((float) gIDX_Motor.GetPosition / gIDX_Motor.UmPerPulse));
#else
                USR_ShowPosition((int32_t) ((float) gIDX_Motor.SetPosition / gIDX_Motor.UmPerPulse));
#endif
              WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);
              break;
            }
          }
          else if (USR_ButtonPressed(BtnMenu,USR_SHORTPRESSTIME,1)==1)
          {
            gUSR_SetMessage("","","     CANCEL SCRAPING","","","");
            USR_ShowScreen(4);  
            WRK_SetStatus(SubStatus, WAITFORINDEXHOME);
          }
          break;
        }
        default:
          break;
      }
      break;
    }
    case EXECUTECOMMAND:
    {
      switch (gWRK_Status.SubStatus)
      {
        case UNDEFINED:
        {      
          USR_ShowScreen(gCurrentScreen * 100 + 1);
          WRK_SetStatus(SubStatus,WAITFORUSER);
          break;
        }
        case WAITFORUSER: //Execute command
        {
          if (USR_ButtonPressed(BtnMenu,USR_SHORTPRESSTIME,1)==1)
            WRK_HandleCommand(0);
					else if (USR_ButtonPressed(BtnOk,USR_SHORTPRESSTIME,1)==1) 
            WRK_HandleCommand(1);
          else if (USR_ButtonPressed(BtnLeft, USR_SHORTPRESSTIME,1)==1)
            WRK_HandleCommand(2);
          else if (USR_ButtonPressed(BtnRight, USR_SHORTPRESSTIME,1)==1)
            WRK_HandleCommand(3);
          else if (USR_ButtonPressed(BtnUp,USR_SHORTPRESSTIME,1)==1)
            WRK_HandleCommand(4);
          else if (USR_ButtonPressed(BtnDown, USR_SHORTPRESSTIME,1)==1)
            WRK_HandleCommand(5);
          break;
        }
        default:
          break;
      }
      break;
    }
    case ENTERVALUE:
    {
      switch (gWRK_Status.SubStatus)
      {
        case UNDEFINED:
        {      
          USR_EnterValue(gCurrentScreen - 10101);
          WRK_SetStatus(SubStatus,WAITFORUSER);
          break;
        }
        case WAITFORUSER:
        {
          if (USR_ButtonPressed(BtnOk,10,1)==1) //Save and return to menu
          {
            USR_SaveParameter();
            USR_ShowScreen (LastScreen); 
            WRK_SetStatus(MainStatus,ACTIVE);
            WRK_SetStatus(SubStatus,WAITFORUSER);
          }
          else if (USR_ButtonPressed(BtnMenu,10,1)==1) //Cancel, don't save
          {
            USR_ShowScreen (LastScreen); 
            WRK_SetStatus(MainStatus,ACTIVE);
            WRK_SetStatus(SubStatus,WAITFORUSER);
          }
          else if (USR_ButtonPressed(BtnLeft, USR_SHORTPRESSTIME,1)==1) //Move cursor left if allowed
          {
            USR_CursorLeft();
          }
          else if (USR_ButtonPressed(BtnRight, USR_SHORTPRESSTIME,1)==1) //Move cursor left if allowed
          {
            USR_CursorRight();
          }
          else if (USR_ButtonPressed(BtnUp,USR_SHORTPRESSTIME,0)==1) //Increase the value if allowed
          {
            USR_CursorUp();
          }
          else if (USR_ButtonPressed(BtnDown, USR_SHORTPRESSTIME,0)==1) //Decrease the value if allowed
          {
            USR_CursorDown();
          }
          break;
        }
        default:
          break;
      }
      break;
    }
    case SCRAPENOSIDESTEPS:
    {
      switch (gWRK_Status.SubStatus)
      {
        case UNDEFINED:
        {
          WRK_SetStatus(SubStatus, WAITFORSTROKEMOTORSTART);
          break;
        }
        case WAITFORSTROKEMOTORSTART:
        {
          if (STR_Set(START,gMachineType[gMachine].Parameters[2].Value*30) == READY)
          {
            STR_Set(UNDEFINED,0);
            gUSR_SetMessage("","SCRAPING THE REED.","","OK: STOP SCRAPING","","OK");
            USR_ShowScreen(4);
            WRK_SetStatus(SubStatus, WAITFORUSER);
          }
          break;
        }
        case WAITFORUSER:
        {
          if (USR_ButtonPressed(BtnOk,1,1)==1)
          {
            gUSR_SetMessage("","","     STOP SCRAPING","","","");
            USR_ShowScreen(4);
            WRK_SetStatus(SubStatus, WAITFORSTROKEMOTORSTOP);
          }
          break;
        }
        case WAITFORSTROKEMOTORSTOP:
        {
          if (STR_Set(GOTOSTARTPOSITION,0) == READY)
          {
            STR_Set(UNDEFINED,0);
            WRK_SetStatus(MainStatus, ACTIVE);
            WRK_SetStatus(SubStatus, WAITFORUSER);
            USR_ShowScreen(gReturnScreen);
          }  
          break;
        }
        default:
          break;
			}
      break;
    }
    
    case ACTIVE:
    {
      switch (gWRK_Status.SubStatus)
      {
        case ENCODERTEST:
        {
          switch (CntStep)
          {
            case 0:  
            case 1:
            { 
              gIDX_Motor.MainStatus = ACTIVE;
              gIDX_Motor.SetPosition = 5000;
              CntStep = 2;
              break;
            }
            case 2:
            {
              if (abs(gIDX_Motor.GetPosition - 5000) < 3)
              {
                gIDX_Motor.MainStatus = ACTIVE;
                gIDX_Motor.SetPosition = 0;  
                CntStep = 3;
              }
              break;
            }
            case 3:
            {
              if (abs(gIDX_Motor.GetPosition) < 3)
              {
                gIDX_Motor.MainStatus = ACTIVE;
                gIDX_Motor.SetPosition = 5000;  
                CntStep = 2;
              }
              break;
            }
            default:
              break;
          }
          
          break;
        }
        case UNDEFINED:
        {
            WRK_SetStatus(SubStatus,WAITFORSPLASHSCREEN);
            break;
        }
        case WAITFORSPLASHSCREEN:
        {
          if (gCounter.Sequence > SPLASHSCREENTIME)
          {
            USR_ClearScreen(0);
            USR_ClearScreen(2);
            USR_ShowScreen(10); //HOME
            gCounter.Sequence = 0;
            WRK_SetStatus(SubStatus,WAITFORUSER);
          }
          break;
        }
        case WAITFORINDEXHOME:
        case WAITFORINDEXSTART:
        {
          if (IDX_Set(HOME,0)== READY)
          {
            IDX_Set(UNDEFINED,0);
            if (gWRK_Status.SubStatus == WAITFORINDEXSTART)
            {
              WRK_SetStatus(SubStatus,WAITFORUSER);
              USR_ShowScreen(gReturnScreen); 
            }
            else
            {
              WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORHOME);
            }
          }
          break;
        }
        case WAITFORSTROKEMOTORHOME:
        {
          if (STR_Set(HOME,0)== READY)
          {
            STR_Set(UNDEFINED,0);
            if (gCurrentScreen > 10200)
            {
              WRK_SetStatus(MainStatus,EXECUTECOMMAND);  
              USR_ShowScreen(gCurrentScreen + 1);
              WRK_SetStatus(SubStatus,WAITFORUSER);  
            }
            else
            {
              WRK_SetStatus(SubStatus,WAITFORUSER);
              USR_ShowScreen(gReturnScreen); 
            }
          }
          break;
        }
        case WAITFORSTROKEMOTORSTART:
        {
          if (STR_Set(GOTOSTARTPOSITION,0)== READY)
          {
            STR_Set(UNDEFINED,0);
            if (gCurrentScreen > 10200)
            {
              USR_ShowScreen (gCurrentScreen/100); 
              WRK_SetStatus(MainStatus,ACTIVE);
              WRK_SetStatus(SubStatus,WAITFORUSER);
            }
            else if (gIDX_Motor.IsInStartPosition == 0) 
            {  
              WRK_SetStatus(SubStatus,WAITFORINDEXSTART);
            }
            else
            {
              WRK_SetStatus(SubStatus,WAITFORUSER);
              USR_ShowScreen(gReturnScreen); 
            }
          }
          break;
        }
        case WAITFORUSER:
        {
					if (USR_ButtonPressed(BtnOk,1,1)==1)
          {
            if ((gCurrentScreen >= 100)&&(gCurrentScreen < 10200))
            {
              if (USR_ButtonPressed(BtnMenu,5000,1) == 1)
              { 
                gServiceMenu = 1 - gServiceMenu;
                if (gServiceMenu)
                  USR_ShowScreen (101);
                else
                  USR_ShowScreen (10101);
              }
            }
            else if (gCurrentScreen == 3) //Error screen
            {
              USR_ShowScreen (gReturnScreen);
            }
            else if (gCurrentScreen == 10) //Find HOME screen
            {
              gReturnScreen = 20;
              gUSR_SetMessage("","","     PLEASE WAIT","","","");
              USR_ShowScreen(4);
              WRK_SetStatus(SubStatus,WAITFORINDEXHOME);  
            }
            else if (gCurrentScreen == 20) //Goto START screen
            {
              gReturnScreen = gLastScrapeScreen;
              gUSR_SetMessage("","","     PLEASE WAIT","","","");
              USR_ShowScreen(4);
              WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);  
            }
            else if (gCurrentScreen == 30) //Start Scrape process with big side step
            {
              gReturnScreen = gCurrentScreen;
              gScrape.SideStep = gMachineType[gMachine].Parameters[4].Value * 84 / 15; //Pulses -> Value * 10 * 840 / 1500
              gScrape.StartPosition = gMachineType[gMachine].Parameters[0].Value * 42 / 15; //Pulses -> Value * 10 / 2 (Half scrape width) * 840 / 1500 um -> Pulse
              gScrape.EndPosition = 0;
              WRK_SetStatus(MainStatus, SCRAPEREED);
            }
            else if (gCurrentScreen == 31) //Start Scrape process with small side step
            {
              gReturnScreen = gCurrentScreen;
              gScrape.SideStep = gMachineType[gMachine].Parameters[3].Value * 84 / 15; //Pulses -> Value * 10 * 840 / 1500
              gScrape.StartPosition = gMachineType[gMachine].Parameters[0].Value * 42 / 15; //Pulses -> Value * 10 / 2 (Half scrape width) * 840 / 1500 um -> Pulse
              gScrape.EndPosition = 0;
              WRK_SetStatus(MainStatus, SCRAPEREED);
            }
            else if (gCurrentScreen == 32) //Start Scrape process only outer sections
            {
              gScrape.SideStep = gMachineType[gMachine].Parameters[3].Value * 84 / 15; //Pulses -> Value * 10 * 840 / 1500
              gScrape.StartPosition = gMachineType[gMachine].Parameters[0].Value * 42 / 15; //Pulses -> Value * 10 / 2 (Half scrape width) * 840 / 1500 um -> Pulse
              gScrape.EndPosition = gMachineType[gMachine].Parameters[1].Value * 42 / 15; //Pulses -> Value * 10 / 2 (Half scrape width inner) * 840 / 1500 um -> Pulse;
              WRK_SetStatus(MainStatus, SCRAPEREED);
            }
            else if (gCurrentScreen == 33) //Start Scrape process only inner sections
            {
              gScrape.SideStep = gMachineType[gMachine].Parameters[3].Value * 84 / 15; //Pulses -> Value * 10 * 840 / 1500
              gScrape.StartPosition = gMachineType[gMachine].Parameters[1].Value * 42 / 15; //Pulses -> Value * 10 / 2 (Half scrape width) * 840 / 1500 um -> Pulse
              gScrape.EndPosition = 0;              
              WRK_SetStatus(MainStatus, SCRAPEREED);
            }
            else if (gCurrentScreen == 34) //Start Scrape process without side steps
            {
              gScrape.SideStep = 0;
              gReturnScreen = gCurrentScreen;
              WRK_SetStatus(MainStatus, SCRAPENOSIDESTEPS);
            }
          }
          else if (USR_ButtonPressed(BtnRight,1,1) == 1)
          {
            if (gCurrentScreen < 100) //Normal screen
            {
              if (gCurrentScreen == 20)
                USR_ShowScreen (gLastScrapeScreen);
              else
                USR_ShowScreen (gCurrentScreen + 10);
              
            }
            else if (gCurrentScreen < 10000) //Menu screen
            {
              USR_ShowScreen ((gCurrentScreen  * 100) + 1);
            } 
            else if ((gCurrentScreen > 10100) && (gCurrentScreen < 10200)) //Parameter screen
            {
              LastScreen = gCurrentScreen;
              WRK_SetStatus(MainStatus,ENTERVALUE);
            }
            else //Command screen 
            {
              LastScreen = gCurrentScreen;
              WRK_SetStatus(MainStatus,EXECUTECOMMAND);
              break;
            }
          }
          else if (USR_ButtonPressed(BtnLeft, USR_SHORTPRESSTIME,1) == 1)
          {
            if (gCurrentScreen < 100) //Normal screen
            {
              if (gCurrentScreen == 10)
                USR_ShowScreen (gLastScrapeScreen);
              else
                USR_ShowScreen (gCurrentScreen - 10);
            }
          }
          else if (USR_ButtonPressed(BtnDown, USR_SHORTPRESSTIME,1) == 1)
          {
            USR_ShowScreen (gCurrentScreen + 1);
          }
          else if (USR_ButtonPressed(BtnUp,USR_SHORTPRESSTIME,1) == 1)
          {
            USR_ShowScreen (gCurrentScreen - 1);
          }
          else if (USR_ButtonPressed(BtnMenu,USR_SHORTPRESSTIME,1) == 1)
          {
            USR_ClearScreen(2);
            if (gCurrentScreen < 100) //Normal screen
            {
              LastNormalScreen = gCurrentScreen;
              USR_ShowScreen (101);
            }
            else if ((gCurrentScreen >= 10000))
            {
              USR_ShowScreen (gCurrentScreen  / 100);
            } 
            else
            {
              USR_ShowScreen (LastNormalScreen);
            }
          }
          break;
        }
        default:
          break;
      }
      break;
    }
    default:
        break;
  }
}

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//! \file       ssd1306.c
//! \author     R. Weimar
//! \brief      Contains routines for the display controller
//! \details    
//! \Attention  
//-----------------------------------------------------------------------------
#include "ssd1306.h"
#include "i2c.h"
#include "main.h"
//-----------------------------------------------------------------------------
// Screenbuffer
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
// Screen object
static SSD1306_t SSD1306;
//-----------------------------------------------------------------------------
//! \brief      Writes a command to the I2C buffer
//! \details    Send a byte to the command register
//! \param[in]  I2C_HandleTypeDef *hi2c
//! \param[in]  uint8_t command
//! \param[out] uint8_t response HAL_OK, HAL_BUSY, HAL_ERROR
HAL_StatusTypeDef ssd1306_WriteCommand(I2C_HandleTypeDef *hi2c, uint8_t command)
{
  return HAL_I2C_Mem_Write(hi2c, SSD1306_I2C_ADDR, 0x00, 1, &command, 1, 10);
}
//-----------------------------------------------------------------------------
//! \brief      Sets the display contrast
//! \details    Sends the contrast command to the display
//! \param[in]  I2C_HandleTypeDef *hi2c
//! \param[in]  uint8_t NewContrast
//! \param[out] uint8_t response 0=Ok, 1=Failed
uint8_t ssd1306_SetContrast(I2C_HandleTypeDef *hi2c,uint8_t NewContrast)
{
  uint8_t status = 0;
  
  status += ssd1306_WriteCommand(&hi2c1, 0x81);   // set contrast control register
  status += ssd1306_WriteCommand(&hi2c1, NewContrast);
  if (status > 0)
    return 1;
  else
    return 0;
}

//-----------------------------------------------------------------------------
//! \brief      Initializes the OLED display
//! \details    Set the required bits
//! \param[in]  I2C_HandleTypeDef *hi2c
//! \param[out] uint8_t response 0=Ok, 1=Failed
uint8_t ssd1306_Init(I2C_HandleTypeDef *hi2c)
{
  // Wait for the screen to boot
  HAL_Delay(100);
  uint8_t status = 0;
  // Init LCD
  status += ssd1306_WriteCommand(hi2c, 0xAE);   // Display off
  status += ssd1306_WriteCommand(hi2c, 0x20);   // Set Memory Addressing Mode
  status += ssd1306_WriteCommand(hi2c, 0x10);   // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
  status += ssd1306_WriteCommand(hi2c, 0xB0);   // Set Page Start Address for Page Addressing Mode,0-7
  status += ssd1306_WriteCommand(hi2c, 0xC8);   // Set COM Output Scan Direction
  status += ssd1306_WriteCommand(hi2c, 0x00);   // Set low column address
  status += ssd1306_WriteCommand(hi2c, 0x10);   // Set high column address
  status += ssd1306_WriteCommand(hi2c, 0x40);   // Set start line address
  status += ssd1306_WriteCommand(hi2c, 0x81);   // set contrast control register
  status += ssd1306_WriteCommand(hi2c, 0xFF);
  status += ssd1306_WriteCommand(hi2c, 0xA1);   // Set segment re-map 0 to 127
  status += ssd1306_WriteCommand(hi2c, 0xA6);   // Set normal display
  status += ssd1306_WriteCommand(hi2c, 0xA8);   // Set multiplex ratio(1 to 64)
  status += ssd1306_WriteCommand(hi2c, SSD1306_HEIGHT - 1);
  status += ssd1306_WriteCommand(hi2c, 0xA4);   // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
  status += ssd1306_WriteCommand(hi2c, 0xD3);   // Set display offset
  status += ssd1306_WriteCommand(hi2c, 0x00);   // No offset
  status += ssd1306_WriteCommand(hi2c, 0xD5);   // Set display clock divide ratio/oscillator frequency
  status += ssd1306_WriteCommand(hi2c, 0xF0);   // Set divide ratio
  status += ssd1306_WriteCommand(hi2c, 0xD9);   // Set pre-charge period
  status += ssd1306_WriteCommand(hi2c, 0x22);
  status += ssd1306_WriteCommand(hi2c, 0xDA);   // Set com pins hardware configuration
#ifdef SSD1306_COM_LR_REMAP
  status += ssd1306_WriteCommand(hi2c, 0x32);   // Enable COM left/right remap
#else
  status += ssd1306_WriteCommand(hi2c, 0x12);   // Do not use COM left/right remap
#endif // SSD1306_COM_LR_REMAP
  status += ssd1306_WriteCommand(hi2c, 0xDB);   // Set vcomh
  status += ssd1306_WriteCommand(hi2c, 0x20);   // 0x20,0.77xVcc
  status += ssd1306_WriteCommand(hi2c, 0x8D);   // Set DC-DC enable
  status += ssd1306_WriteCommand(hi2c, 0x14);   //
  status += ssd1306_WriteCommand(hi2c, 0xAF);   // Turn on SSD1306 panel
  if (status != 0) return 1;
  // Clear screen
  ssd1306_Fill(Black);
  // Flush buffer to screen
  ssd1306_UpdateScreen(hi2c);
  // Set default values for screen object
  SSD1306.CurrentX = 0;
  SSD1306.CurrentY = 0;
  SSD1306.Initialized = 1;
  return 0;
}
//-----------------------------------------------------------------------------
//! \brief      Fill the whole screen with the given color
//! \details    Fill the whole screen with the given color
//! \param[in]  SSD1306_COLOR color
void ssd1306_Fill(SSD1306_COLOR color)
{
  // Fill screenbuffer with a constant value (color)
  uint32_t i;
  for(i = 0; i < sizeof(SSD1306_Buffer); i++)
  {
    SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
  }
}
//-----------------------------------------------------------------------------
//! \brief      Update the screen
//! \details    Write the screenbuffer with changes to the screen
//! \param[in]  I2C_HandleTypeDef *hi2c
void ssd1306_UpdateScreen(I2C_HandleTypeDef *hi2c)
{
  uint8_t i;
  for (i = 0; i < 8; i++) 
  {
    ssd1306_WriteCommand(hi2c, 0xB0 + i);
    ssd1306_WriteCommand(hi2c, 0x00);
    ssd1306_WriteCommand(hi2c, 0x10);
    HAL_I2C_Mem_Write(hi2c, SSD1306_I2C_ADDR, 0x40, 1, &SSD1306_Buffer[SSD1306_WIDTH * i], SSD1306_WIDTH, 100);
  }
}
//-----------------------------------------------------------------------------
//! \brief      Update one page of the screen
//! \details    Write one page of the screenbuffer with changes to the screen
//! \param[in]  I2C_HandleTypeDef *hi2c
//! \param[in]  uint8_t newPage
void ssd1306_UpdateScreenPage(I2C_HandleTypeDef *hi2c, uint8_t newPage)
{
	ssd1306_WriteCommand(hi2c, 0xB0 + newPage);
	ssd1306_WriteCommand(hi2c, 0x00);
	ssd1306_WriteCommand(hi2c, 0x10);
	HAL_I2C_Mem_Write(hi2c, SSD1306_I2C_ADDR, 0x40, 1, &SSD1306_Buffer[SSD1306_WIDTH * newPage], SSD1306_WIDTH, 100);
}
//-----------------------------------------------------------------------------
//! \brief      Draw a pixel on the screen
//! \details    Draw one pixel in the screenbuffer
//! \param[in]  uint8_t x  -> X Coordinate
//! \param[in]  uint8_t y  -> Y Coordinate
//! \param[in]  SSD1306_COLOR color -> Pixel color (Balck or White)
//! \param[in]  uint8_t newPage
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color)
{
  if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
  {
    // Don't write outside the buffer
    return;
  }
  // Check if pixel should be inverted
  if (SSD1306.Inverted)
  {
    color = (SSD1306_COLOR)!color;
  }
  // Draw in the correct color
  if (color == White)
  {
    SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
  }
  else
  {
    SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
  }
}
//-----------------------------------------------------------------------------
//! \brief      Draw 1 char on the screen 
//! \details    Draw 1 char to the screen buffer
//! \param[in]  char ch -> Character to write
//! \param[in]  FontDef Font -> Selected font
//! \param[in]  SSD1306_COLOR color -> Character color (Black or white)
//! \param[out] char response 0=Failed, other = OK
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color)
{
  int32_t i, j;
  uint32_t b;
  // Check remaining space on current line
  /*if (SSD1306_WIDTH <= (SSD1306.CurrentX + Font.FontWidth) ||
      SSD1306_HEIGHT <= (SSD1306.CurrentY + Font.FontHeight))
  {
    // Not enough space on current line
    return 0;
  }
  */
  // Translate font to screenbuffer
  for (i = -1; i < Font.FontHeight; i++)
  {
    if (i == -1)
    {
      for (j = 0; j < Font.FontWidth; j++)
      {
        ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) !color);
      }
    }
    else
    {
      b = Font.data[(ch - 32) * Font.FontHeight + i];
      for (j = 0; j < Font.FontWidth; j++)
      {
        if ((b << j) & 0x8000)
        {
          ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
        }
        else
        {
          ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) !color);
        }
      }
    }
  }

  // The current space is now taken
  SSD1306.CurrentX += Font.FontWidth;

  // Return written char for validation
  return ch;
}
//-----------------------------------------------------------------------------
//! \brief      Draw text on the screen 
//! \details    Write full string to screenbuffer
//! \param[in]  char* str -> String to write
//! \param[in]  FontDef Font -> Selected font
//! \param[in]  SSD1306_COLOR color -> Character color (Black or white)
//! \param[out] char response *str -> Last character sent
char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color)
{
  // Write until null-byte
  while (*str)
  {
    if (ssd1306_WriteChar(*str, Font, color) != *str)
    {
      // Char could not be written
      return *str;
    }
    // Next char
    str++;
  }
  // Everything ok
  return *str;
}
//! \brief      Inverts the inverted flag of the screen 
//! \details    Invert background/foreground colors
//! \param      None
void ssd1306_InvertColors(void)
{
    SSD1306.Inverted = !SSD1306.Inverted;
}
//-----------------------------------------------------------------------------
//! \brief      Gets the inverted flag of the screen 
//! \details    Get Invert background/foreground colors
//! \param[out] uint8_t result -> 0 = not inverted, 1 = inverted
uint8_t ssd1306_GetInvertColors(void)
{
  return SSD1306.Inverted;
}
//-----------------------------------------------------------------------------
//! \brief      Sets the cursor position on the screen 
//! \details    Sets the pointer for the cursor in the screen buffer
//! \param[in]  uint8_t newX -> X top 
//! \param[in]  uint8_t newY -> Y left
void ssd1306_SetCursor(uint8_t newX, uint8_t newY)
{
	#if (SSD1306_WIDTH == 130)
		SSD1306.CurrentX = newX+2;
	#else
		SSD1306.CurrentX = newX;
	#endif
    SSD1306.CurrentY = newY;
}
//-----------------------------------------------------------------------------
//! \brief      Draws the logo on the screen
//! \details    Draws reed machines logoin te screen buffer
//! \param[in]  SSD1306_COLOR color -> Logo color (Black or white)
void ssd1306_DrawLogo (SSD1306_COLOR color)
{
  uint8_t i,j,k;
  uint8_t EndColumn;
  uint32_t b;

  
  for (i = 0; i < 43; i++)
  {
    for (k = 0; k<3; k++)
    {
      b = Logo37x43[i*4+k];
      if (k==2)
        EndColumn = 5;
      else
        EndColumn = 16;
      for (j = 0; j < EndColumn; j++)
      {
        if ((b << j) & 0x8000)
        {
          ssd1306_DrawPixel(SSD1306.CurrentX + j + 16*k, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
        }
        else
        {
          ssd1306_DrawPixel(SSD1306.CurrentX + j +16*k, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
        }
      }
    }
  }
}
//-----------------------------------------------------------------------------
//! \brief      Draws a battery symbol on the screen
//! \details    Draws a battery symbol and status in te screen buffer
//! \param[in]  SSD1306_COLOR color -> Battery symbol color (Black or white)
//! \param[in]  uint8_t newBars -> Nr of bars to show (0, 1, 2 or 3)
//! \param[in]  uint8_t newX -> X top 
//! \param[in]  uint8_t newY -> Y left
void ssd1306_DrawBattery (SSD1306_COLOR color, uint8_t newBars, uint8_t newX, uint8_t newY)
{
  uint32_t b;
  ssd1306_SetCursor(newX,newY);
  for (uint8_t i = 0; i < 7; i++)
  {
    b = Battery[((newBars) * 7) + i];
    for (uint8_t j = 0; j < 13; j++)
    {
      if ((b << j) & 0x8000)
      {
        ssd1306_DrawPixel(SSD1306.CurrentX + j , (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
      }
      else
      {
        ssd1306_DrawPixel(SSD1306.CurrentX + j , (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
      }
    }
  }
}
//-----------------------------------------------------------------------------
//! \brief      Draws a rectangle on the screen with chamfer
//! \details    Draws a rectangle on the screen with chamfer
//! \param[in]  SSD1306_COLOR color -> Rectangle color (Black or white)
//! \param[in]  uint8_t newX -> X top 
//! \param[in]  uint8_t newY -> Y left
//! \param[in]  uint8_t width -> width
//! \param[in]  uint8_t height -> height
//! \param[in]  uint8_t Chamfer -> Chamfer
void ssd1306_DrawRectangle(SSD1306_COLOR color, uint8_t newX, uint8_t newY, uint8_t width, uint8_t height, uint8_t Chamfer)
{
  uint8_t Ymin = newY;
  uint8_t Heightmax = height;
  
  
  ssd1306_SetCursor(newX,newY);
  for (uint8_t col = 0; col < width;col++)
  {
    if (col < Chamfer)
      Ymin = Chamfer - col;
    else if (col >= width - Chamfer) 
      Ymin = Chamfer + 1 - (width - col);
    else
      Ymin = 0;
    Heightmax= (height - Ymin);
    for (uint8_t row = Ymin; row < Heightmax ;row++)
    {
      ssd1306_DrawPixel(SSD1306.CurrentX + col , (SSD1306.CurrentY + row), (SSD1306_COLOR) color);
    }
  }
}
//-----------------------------------------------------------------------------
//! \brief      Draw 1 char on the screen 
//! \details    Draw 1 char to the screen buffer
//! \param[in]  char ch -> Character to write
//! \param[in]  FontDefEightBit Font -> Selected font
//! \param[in]  SSD1306_COLOR color -> Character color (Black or white)
//! \param[out] char response 0=Failed, other = OK
char ssd1306_WriteCharEightBitFont(char ch, FontDefEightBit Font, SSD1306_COLOR color)
{
    int32_t i, j;
    uint32_t b;
    // Translate font to screenbuffer
    for (i = 0; i < Font.FontHeight; i++)
    {
      b = Font.data[(ch - 32) * Font.FontHeight + i];
      for (j = 0; j < Font.FontWidth; j++)
      {
          if ((b << j) & 0x80)
          {
              ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
          }
          else
          {
              ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) !color);
          }
      }

    }
    // The current space is now taken
    SSD1306.CurrentX += Font.FontWidth;

    // Return written char for validation
    return ch;
}
//-----------------------------------------------------------------------------
//! \brief      Draw text on the screen 
//! \details    Write full string to screenbuffer
//! \param[in]  char* str -> String to write
//! \param[in]  FontDefEightBit Font -> Selected font
//! \param[in]  SSD1306_COLOR color -> Character color (Black or white)
//! \param[out] char response *str -> Last character sent

char ssd1306_WriteStringEightBitFont(char* str, FontDefEightBit Font, SSD1306_COLOR color)
{
    // Write until null-byte
    while (*str)
    {
        if (ssd1306_WriteCharEightBitFont(*str, Font, color) != *str)
        {
            // Char could not be written
            return *str;
        }

        // Next char
        str++;
    }
    // Everything ok
    return *str;
}


